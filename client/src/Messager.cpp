#include "Messager.hpp"

string server_addr = "localhost";
int server_port = 10025;

void set_server_addr(struct sockaddr_in * serv_addr, const char * addr, const char * port) {
  int errcode;
  struct addrinfo hints, *result;

  memset(&hints, 0, sizeof (hints));

  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  errcode = getaddrinfo (addr, NULL, &hints, &result);

  if (errcode != 0) {
    perror ("getaddrinfo");
    return;
  }

  serv_addr->sin_family = AF_INET;
  struct sockaddr_in * temp_addr = (struct sockaddr_in *) result->ai_addr;
  serv_addr->sin_addr = temp_addr->sin_addr; 
  serv_addr->sin_port = htons(atoi(port));
}

int send_message(cJSON *message) {
    // create socket 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "ERROR, no such host" << endl;
        return -1;
    }

    // get server address
    struct hostent *server = gethostbyname(server_addr.c_str());
    if (server == NULL) {
        cerr << "ERROR, no such host" << endl;
        return -1;
    }

    // create socket address
    struct sockaddr_in serv_addr;
    set_server_addr(&serv_addr, server_addr.c_str(), to_string(server_port).c_str());

    // connect to server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "ERROR during connection" << endl;
        return -1;
    }

    // send message
    string message_str = cJSON_Print(message);
    int n = write(sockfd, message_str.c_str(), message_str.length());
    if (n < 0) {
        cerr << "ERROR writing to socket" << endl;
        return -1;
    }

    // TODO maybe wait confirm from server

    // close socket
    close(sockfd);
    cJSON_Delete(message);
    return n;
}