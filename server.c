// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    unsigned int nponto;
    float value;
    char validity[10];
    char source[10];
    char detail[10];
    time_t timestamp;
} AnalogDataPoint;

void serializeAnalogDataPoint(char *buffer, const AnalogDataPoint *dataPoint) {
    sprintf(buffer, "%u|%f|%s|%s|%s|%ld", 
            dataPoint->nponto, dataPoint->value, 
            dataPoint->validity, dataPoint->source, 
            dataPoint->detail, dataPoint->timestamp);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    AnalogDataPoint dataPoint = {12345, 10.5, "Good", "Processed", "Normal", time(NULL)};
    serializeAnalogDataPoint(buffer, &dataPoint);

    send(new_socket, buffer, strlen(buffer), 0);
    printf("Analog data point sent\n");

    close(server_fd);
    return 0;
}
