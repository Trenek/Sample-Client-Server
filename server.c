#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 28015
#define BUFFER_SIZE 1024

int main() {
    int server_fd
    int client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Nie utworzono gniazda");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Nie przypisano poprawnie");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 3);
    printf("Serwer nasluchuje na porcie %d...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int bytes_received = read(client_fd, buffer, BUFFER_SIZE);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Odebrano wiadomosc: %s\n", buffer);

        char *ack = "ACK";
        send(client_fd, ack, strlen(ack), 0);
        printf("Potwierdzenie wys³ane.\n");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
