#include "Worker.hpp"

cJSON * create_confirm_json() {
    cJSON * json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "type", "confirm");
    return json;
}

void Worker::operator()(int socket, sockaddr_in & client_addr,
unordered_map<string, int> & gpio_out_values) {
    // read request from accepted socket
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytes_read == -1) {
        perror("recv");
        return;
    }
    // parse request as json
    string request = string(buffer, bytes_read);
    cJSON * root = cJSON_Parse(request.c_str());
    if (root == NULL) {
        cerr << "Error parsing json: " << cJSON_GetErrorPtr() << endl;
        return;
    }
    // if type is action apply to gpio_out
    if (cJSON_GetObjectItem(root, "type")->valuestring == "action") {
        cJSON * action = cJSON_GetObjectItem(root, "action");
        // get tag and value
        string tag = cJSON_GetObjectItem(action, "tag")->valuestring;
        int value = cJSON_GetObjectItem(action, "value")->valueint;
        // apply value to gpio_out
        gpio_out_values[tag] = value;
        // send confirm to client
        cJSON * confirm = create_confirm_json();
        string json_string = cJSON_Print(confirm);
        send(socket, json_string.c_str(), json_string.length(), 0);
        cJSON_Delete(confirm);
    }
    close(socket);
    cJSON_Delete(root);
    
}