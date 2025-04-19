#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 28015
#define BUFFER_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };
    char *message = "Test";
    char buffer[BUFFER_SIZE] = {};

    if (sock < 0) {
        perror("Nie utworzono gniazda");
        return -1;
    }

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Nie prawidlowy adres");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Blad polaczenia");
        return -1;
    }

    send(sock, message, strlen(message), 0);
    printf("Wyslano: %s\n", message);

    int bytes = read(sock, buffer, BUFFER_SIZE);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Otrzymano potwierdzenie od serwera: %s\n", buffer);
    }

    close(sock);
    return 0;
}
