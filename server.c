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
    char destination[20];
    char messageType[20];
    char protocolVersion[10];
} AnalogDataPoint;

void serializeAnalogDataPoint(char *buffer, const AnalogDataPoint *dataPoint) {
    sprintf(buffer, "%u|%f|%s|%s|%s|%ld|%s|%s|%s", 
            dataPoint->nponto, dataPoint->value, 
            dataPoint->validity, dataPoint->source, 
            dataPoint->detail, dataPoint->timestamp,
            dataPoint->destination, dataPoint->messageType, 
            dataPoint->protocolVersion);
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

    AnalogDataPoint dataPoint = {
        .nponto = 12345,
        .value = 10.5,
        .validity = "Good",
        .source = "SubX",
        .detail = "Normal",
        .timestamp = time(NULL),
        .destination = "Control Center",
        .messageType = "Measurement",
        .protocolVersion = "1.0"
    };

    serializeAnalogDataPoint(buffer, &dataPoint);
    send(new_socket, buffer, strlen(buffer), 0);

    char formattedTime[30];
    strftime(formattedTime, 30, "%Y-%m-%d %H:%M:%S", localtime(&dataPoint.timestamp));

    printf("ICCP Analog Data Point Sent\n");
    printf("======================================================\n");
    printf("Transaction ID: %u\n", dataPoint.nponto);
    printf("Data Value: %.2f V\n", dataPoint.value);
    printf("Data Quality: %s\n", dataPoint.validity);
    printf("Source Substation: %s\n", dataPoint.source);
    printf("Data Details: %s\n", dataPoint.detail);
    printf("Timestamp: %s\n", formattedTime);
    printf("Destination: %s\n", dataPoint.destination);
    printf("Message Type: %s\n", dataPoint.messageType);
    printf("ICCP Protocol Version: %s\n", dataPoint.protocolVersion);
    printf("======================================================\n");

    close(new_socket);
    close(server_fd);

    return 0;
}
