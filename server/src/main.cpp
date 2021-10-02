#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <streambuf>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <unordered_map>
#include <thread>
using namespace std;

#include "cJSON.h"
#include "Types.hpp"
#include "Router.hpp"
#include "Interface.hpp"

int sockfd;

#define MAX_BUFFER_SIZE 1024

void set_server_addr(struct sockaddr_in * serv_addr, char * addr, char * port) {
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

int is_ready(int fd) {
  fd_set fdset;
  struct timeval timeout;
  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);

  timeout.tv_sec = 0;
  timeout.tv_usec = 1;

  return select(fd+1, &fdset, NULL, NULL, &timeout) == 1;
}


int main(int argc, char *argv[]){
    if (argc != 3) {
        cout << "Usage: ./server <ip> <port>" << endl;
        return 1;
    }
    signal(SIGINT, [](int sig) {
        close(sockfd);
        exit(0);
    });
    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    set_server_addr(&serv_addr, argv[1], argv[2]);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error opening socket" << endl;
        return 1;
    }
    cout << "Iniciando servidor central no Ip: " 
    << inet_ntoa(serv_addr.sin_addr) 
    << " na porta: " << argv[2] << endl;
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Error on binding" << endl;
        return 1;
    }
    
    listen(sockfd, 5);

    Interface interface;

    thread interface_thread(interface);

    interface_thread.detach();

    while(true){
        // accept connections and print to stdout
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            cout << "Error on accept" << endl;
            return 1;
        }

        char* buffer = new char[MAX_BUFFER_SIZE];
        bzero(buffer, MAX_BUFFER_SIZE);
        // TODO - read all data from socket, allocating memory if necessary
        int n = read(newsockfd, buffer, MAX_BUFFER_SIZE);
        if (n < 0) {
            cout << "Error reading from socket" << endl;
            return 1;
        }
        close(newsockfd);
        cJSON * root = cJSON_Parse(buffer);
        if (root == NULL) {
            cout << "Error parsing JSON" << endl;
            return 1;
        }
        string ip = inet_ntoa(cli_addr.sin_addr);
        cJSON_AddStringToObject(root, "ip", ip.c_str());
        Router * router = new Router(root);
        thread * router_thread = new thread(&Router::filterRequest, router);
        router_thread->detach();
    }

    close(sockfd);

    return 0;
}