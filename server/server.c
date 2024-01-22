#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include "cJSON.h"

#define MAX_MSG_LEN 1024


// char* processJSON(char* jsonString){

   

//     // Open the JSON file for reading
   

//     // Parse the JSON string
//     json = cJSON_Parse(jsonString);
//     if (json == NULL) {
//         printf("Error parsing JSON string.\n");
//         fclose(fp);
//     }

//     // Get the JSON string representation
//     char* json_string = cJSON_Print(json);

//     // Free allocated memory
//     fclose(fp);

//     return json_string;

// }

int main() {
    int sockfd;
    char buffer[MAX_MSG_LEN];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    ssize_t num_bytes;

    // Create socket for receiving UDP packets
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up address for receiving UDP packets
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8765);

    // Bind socket to address for receiving UDP packets
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming UDP packets and send response
    while (1) {
        printf("Waiting for UDP packet...\n");
        num_bytes = recvfrom(sockfd, buffer, MAX_MSG_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("Received %zd bytes from %s:%d\n", num_bytes, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        buffer[num_bytes] = '\0';
        printf("Message: %s\n", buffer);
        sendto(sockfd, buffer, num_bytes, 0, (struct sockaddr *)&client_addr, addr_len);
    }

    // Close socket
    close(sockfd);
    return 0;
}

