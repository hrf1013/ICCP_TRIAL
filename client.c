// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 2048

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

void deserializeAnalogDataPoint(const char *buffer, AnalogDataPoint *dataPoint) {
    sscanf(buffer, "%u|%f|%9[^|]|%9[^|]|%9[^|]|%ld|%19[^|]|%19[^|]|%9s", 
           &dataPoint->nponto, &dataPoint->value, 
           dataPoint->validity, dataPoint->source, 
           dataPoint->detail, &dataPoint->timestamp,
           dataPoint->destination, dataPoint->messageType,
           dataPoint->protocolVersion);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    read(sock, buffer, BUFFER_SIZE);
    AnalogDataPoint dataPoint;
    deserializeAnalogDataPoint(buffer, &dataPoint);

    char formattedTime[30];
    strftime(formattedTime, 30, "%Y-%m-%d %H:%M:%S", localtime(&dataPoint.timestamp));

    printf("Received ICCP-like Message:\n");
    printf("=================================================\n");
    printf("Transaction ID: %u\n", dataPoint.nponto);
    printf("Data Value: %.2f V\n", dataPoint.value);
    printf("Data Quality: %s\n", dataPoint.validity);
    printf("Source Substation: %s\n", dataPoint.source);
    printf("Data Details: %s\n", dataPoint.detail);
    printf("Timestamp: %s\n", formattedTime);
    printf("Destination: %s\n", dataPoint.destination);
    printf("Message Type: %s\n", dataPoint.messageType);
    printf("ICCP Protocol Version: %s\n", dataPoint.protocolVersion);
    printf("=================================================\n");

    close(sock);
    return 0;
}
