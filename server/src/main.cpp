#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <streambuf>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <thread>
using namespace std;

#include "cJSON.h"
#include "Types.hpp"
#include "Router.hpp"
#include "RouterSem.hpp"
#include "Interface.hpp"
#include "Control.hpp"
#include "Messager.hpp"

int sockfd;

#define MAX_BUFFER_SIZE 2048

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
  auto * temp_addr = (struct sockaddr_in *) result->ai_addr;
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

    bool is_running = true;

    RouterSem::init();
    messager_sem_init();

    // create main log file
    string filename = "exec_log_" + to_string(time(0)) + ".log";
    ofstream log_file(filename);

    // change cerr and cout to use file instead of stdout
    cerr.rdbuf(log_file.rdbuf());
    cout.rdbuf(log_file.rdbuf());

    Interface interface;

    thread interface_thread(interface, ref(is_running));

    interface_thread.detach();

    Control * control = new Control();

    thread control_thread(&Control::init_control, control);

    control_thread.detach();

    while(is_running){
        // accept connections and print to stdout
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        if(!is_ready(sockfd)) continue; 
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
        RouterSem::acquire();
        Router * router = new Router(root);
        thread * router_thread = new thread(&Router::filterRequest, router);
        router_thread->detach();
    }

    Control::stop_all();

    close(sockfd);

    return 0;
}