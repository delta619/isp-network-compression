#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "cJSON.c"

#define MAX_MSG_LEN 1024

cJSON* json;

char* getJsonValueStringUsingKey(char* key){
    cJSON* value = cJSON_GetObjectItemCaseSensitive(json, key);
    if (value == NULL) {
        printf("its null");
        return NULL;
    }
    if (value->type != cJSON_String) {
        printf("not a string");
        return NULL;  // Key exists, but is not a string
    }
    char* valueStr = cJSON_GetStringValue(value);
    return valueStr;
}

int getJsonValueIntUsingKey(char* key){
    cJSON* value = cJSON_GetObjectItemCaseSensitive(json, key);
    if (value == NULL) {
        printf("its null");
        return -1;
    }
    if (value->type != cJSON_Number) {
        printf("not an int");
        return -1;  // Key exists, but is not a string
    }
    int valueInt = cJSON_GetNumberValue(value);
    return (int)valueInt;
}

char* processJSONFile(char* file){

    FILE* fp;
    long file_size;
    char* file_contents;

    // Open the JSON file for reading
    fp = fopen(file, "rb");
    if (fp == NULL) {
        printf("Error opening file.\n");
    }

    // Determine the size of the file
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    // Allocate memory for the file contents
    file_contents = (char*)malloc((file_size + 1) * sizeof(char));
    if (file_contents == NULL) {
        printf("Error allocating memory for file contents.\n");
        fclose(fp);
    }

    // Read the file contents into memory
    if (fread(file_contents, sizeof(char), file_size, fp) != file_size) {
        printf("Error reading file contents.\n");
        fclose(fp);
        free(file_contents);
    }

    // Null-terminate the string
    file_contents[file_size] = '\0';

    // Parse the JSON string
    json = cJSON_Parse(file_contents);
    if (json == NULL) {
        printf("Error parsing JSON string.\n");
        fclose(fp);
    }

    // Get the JSON string representation
    char* json_string = cJSON_Print(json);

    // Free allocated memory
    fclose(fp);

    return json_string;

}

int main() {
    char* JSON_string = processJSONFile("config.json");

    int sockfd;
    char buffer[MAX_MSG_LEN];
    struct sockaddr_in server_addr, my_addr;

    ssize_t num_bytes;

    // Create socket for sending UDP packets
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // set my config

    int on = IP_PMTUDISC_DO;
    if (setsockopt(sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


    int myport = getJsonValueIntUsingKey("client_port");

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(myport);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
    }




    // load config files

    char* server_address = getJsonValueStringUsingKey("server_address"); // get from json config
    int server_port = getJsonValueIntUsingKey("server_port"); // get from json config

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(server_port);

    // Create buffer of bytes

    size_t json_len = strlen(JSON_string);
    char* byte_buffer = (char*) malloc((json_len + 1) * sizeof(char)); // allocate memory
    strcpy(byte_buffer, JSON_string); // copy JSON string to buffer

    // Send buffer over UDP to other machine
    sendto(sockfd, byte_buffer , strlen(byte_buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Close socket
    close(sockfd);

    return 0;
}

















