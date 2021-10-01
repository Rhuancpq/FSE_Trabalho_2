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

cJSON * create_init_message(string name, int port, 
const vector<GPIO_Pin> & inputs, const vector<GPIO_Pin> & outputs) {
    cJSON * init_message = cJSON_CreateObject();
    cJSON_AddStringToObject(init_message, "type", "init");
    cJSON_AddStringToObject(init_message, "name", name.c_str());
    cJSON_AddNumberToObject(init_message, "port", port);
    // inputs
    cJSON * inputs_array = cJSON_CreateArray();
    for (auto & input : inputs) {
        cJSON * input_object = cJSON_CreateObject();
        cJSON_AddStringToObject(input_object, "tag", input.tag.c_str());
        cJSON_AddStringToObject(input_object, "type", input.type.c_str());
        cJSON_AddItemToArray(inputs_array, input_object);
    }
    // outputs
    cJSON * outputs_array = cJSON_CreateArray();
    for (auto & output : outputs) {
        cJSON * output_object = cJSON_CreateObject();
        cJSON_AddStringToObject(output_object, "tag", output.tag.c_str());
        cJSON_AddStringToObject(output_object, "type", output.type.c_str());
        cJSON_AddItemToArray(outputs_array, output_object);
    }
    cJSON_AddItemToObject(init_message, "inputs", inputs_array);
    cJSON_AddItemToObject(init_message, "outputs", outputs_array);
    return init_message;
}

cJSON * create_leave_message(string name, int port) {
    cJSON * leave_message = cJSON_CreateObject();
    cJSON_AddStringToObject(leave_message, "type", "leave");
    cJSON_AddStringToObject(leave_message, "name", name.c_str());
    cJSON_AddNumberToObject(leave_message, "port", port);
    return leave_message;
}

void Listener::operator()(const bool & is_end, string name, int port,
unordered_map<string, int> & gpio_out_values, 
const vector<GPIO_Pin> & in_gpios, const vector<GPIO_Pin> & out_gpios) {
    int sockfd;

    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error opening socket" << endl;
        return;
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    set_server_addr(&serv_addr, DEFAULT_IP, to_string(port).c_str());
    cout << "Iniciando servidor distribuído no Ip: "
    << inet_ntoa(serv_addr.sin_addr)
    << " na porta: " << port << endl;
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error on binding" << endl;
        return;
    }

    listen(sockfd, 5);

    cJSON * init_message = create_init_message(name, port, in_gpios, out_gpios);
    // TODO Se der erro criar task pra tentar novamente a cada 10 segundos
    send_message(init_message);
    cJSON_Delete(init_message);

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
            Worker worker;
            thread worker_thread(worker, newsockfd, ref(cli_addr),  ref(gpio_out_values));
            worker_thread.detach();
        }
        // Dormir por 50ms
        this_thread::sleep_for(50ms);
    }

    cJSON * leave_message = create_leave_message(name, port);
    send_message(leave_message);
    cJSON_Delete(leave_message);

    close(sockfd);
}