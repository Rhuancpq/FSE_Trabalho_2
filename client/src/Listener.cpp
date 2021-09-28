#include "Listener.hpp"

int is_ready(int fd) {
  fd_set fdset;
  struct timeval timeout;
  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);

  timeout.tv_sec = 0;
  timeout.tv_usec = 1;

  return select(fd+1, &fdset, NULL, NULL, &timeout) == 1;
}

void Listener::operator()(const bool & is_end, string ip, int port) {
    int sockfd;

    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error opening socket" << endl;
        return;
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    set_server_addr(&serv_addr, ip.c_str(), to_string(port).c_str());
    cout << "Iniciando servidor distribuído no Ip: "
    << inet_ntoa(serv_addr.sin_addr)
    << " na porta: " << port << endl;
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error on binding" << endl;
        return;
    }

    listen(sockfd, 5);

    // TODO send message to central server to inform that this server is ready

    while (!is_end) {
        if (is_ready(sockfd)) {
            struct sockaddr_in cli_addr;
            socklen_t clilen = sizeof(cli_addr);
            int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                cerr << "Error on accept" << endl;
                continue;
            }
            cout << "Conectado com o cliente: " << inet_ntoa(cli_addr.sin_addr) << endl;
            // TODO criar thread para tratar conexão

        }
        // Dormir por 50ms
        this_thread::sleep_for(50ms);
    }

    // TODO send message to central server to inform that this server is leaving

    close(sockfd);
}