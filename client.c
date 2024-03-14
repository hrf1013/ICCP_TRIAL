// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
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

void deserializeAnalogDataPoint(const char *buffer, AnalogDataPoint *dataPoint) {
    sscanf(buffer, "%u|%f|%[^|]|%[^|]|%[^|]|%ld", 
           &dataPoint->nponto, &dataPoint->value, 
           dataPoint->validity, dataPoint->source, 
           dataPoint->detail, &dataPoint->timestamp);
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

    // Connecting to the server running on localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    read(sock, buffer, BUFFER_SIZE);
    AnalogDataPoint dataPoint;
    deserializeAnalogDataPoint(buffer, &dataPoint);

    // Client-side output after receiving data
    printf("Received Analog Data Point:\n");
    printf("=========================================\n");
    printf("Nponto: %u\n", dataPoint.nponto);
    printf("Value: %.2f\n", dataPoint.value);
    printf("Validity: %s\n", dataPoint.validity);
    printf("Source: %s\n", dataPoint.source);
    printf("Detail: %s\n", dataPoint.detail);
    printf("Timestamp: %s\n", ctime(&dataPoint.timestamp));
    printf("=========================================\n");

    close(sock);
    return 0;
}
