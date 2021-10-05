#include "Messager.hpp"

sem_t thread_sem;

void messager_sem_init() {
    sem_init(&thread_sem, 0, 5);
}

int send_message(string ip, int port, string message) {
    // open socket and send message
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return -1;
    }
    server = gethostbyname(ip.c_str());
    if (server == NULL) {
        cerr << "ERROR, no such host" << endl;
        close(sockfd);
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cerr << "ERROR connecting" << endl;
        close(sockfd);
        return -1;
    }
    // send message
    const char * message_str = message.c_str();
    int n = write(sockfd,message_str,strlen(message_str));
    if (n < 0) {
        cerr << "ERROR writing to socket" << endl;
        close(sockfd);
        return -1;
    }
    // confirm message was sent
    char buffer[1024];
    bzero(buffer,1024);
    n = read(sockfd,buffer,1024);
    if (n < 0) {
        cerr << "ERROR reading from socket" << endl;
        close(sockfd);
        return -1;
    }
    cJSON * resp = cJSON_Parse(buffer);
    if (resp == NULL) {
        cerr << "Error before: [" << cJSON_GetErrorPtr() << "]" << endl;
        return -1;
    }
    if(string(cJSON_GetObjectItem(resp, "type")->valuestring) == "confirm") {
        cJSON_Delete(resp);
    } else {
        cJSON_Delete(resp);
        close(sockfd);
        return -1;
    }
    // close socket
    close(sockfd);
    sem_post(&thread_sem);
    return 0;
}

void send_message_async(string ip, int port, cJSON * message) {
    // lock semaphore
    sem_wait(&thread_sem);
    cout << "Sending Message to Server: " << ip << ":" << port << endl;
    // create thread to send message
    string message_str = cJSON_Print(message);
    thread(send_message, ip, port, message_str).detach();
}