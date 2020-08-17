#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <pthread.h>
#include <stdlib.h>

enum REQUESTS {REQUEST_READ=1, REQUEST_WRITE, REQUEST_MALLOC, REQUEST_REALLOC, REQUEST_FREE, REQUEST_END};
#define BACKLOG 5
int clientSocket;

void requestHandler() {
    char* where;
    long howMuch;

    char request[40];
    char c;
    int socket = clientSocket;
    printf("Starting, socket %d\n", socket);
    while (read(socket, request, 1)) {
        c = *request;
        printf("Got %c\n", c);
        if (c == REQUEST_WRITE || c == REQUEST_READ) {
            read(socket, request, 16);
            where = (char*)strtoll(request, NULL, 16);
            read(socket, request, 16);
            howMuch = strtoll(request, NULL, 16);
            printf("Where: %llx, how much: %d\n", where, howMuch);

            if (c == REQUEST_WRITE)
                read(socket, where, howMuch);
            else
                send(socket, where, howMuch, MSG_CONFIRM);
        }
        else if (c == REQUEST_MALLOC) {
            read(socket, request, 16);
            howMuch = strtoll(request, NULL, 16);
            sprintf(request, "%016llx", malloc(howMuch));
            send(socket, request, 16, MSG_CONFIRM);
        }
        else if(c == REQUEST_REALLOC) {
            read(socket, request, 16);
            where = (char*)strtoll(request, NULL, 16);
            read(socket, request, 16);
            howMuch = strtoll(request, NULL, 16);
            char* newAlloc = realloc(where, howMuch);
            sprintf(request, "%016llx", newAlloc);
            send(socket, request, 16, MSG_CONFIRM); 
        }
        else if(c == REQUEST_FREE) {
            read(socket, request, 16);
            where = (char*)strtoll(request, NULL, 16);
            free(where);
        }
        else if(c == REQUEST_END) {
            return;
        }
    }
}

void teleAllocServer(int port, int maxConnections) {
    int sock;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    pthread_t threads[maxConnections];
    int clal = sizeof clientAddress; // this is stupid.
    int currentConnections;
    int i;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&serverAddress, sizeof serverAddress);
    
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddress.sin_port = htons(port);


    bind(sock, (struct sockaddr *)&serverAddress, sizeof serverAddress);
    listen(sock, BACKLOG);
    for (currentConnections = 0; currentConnections < maxConnections; currentConnections++) {
        clientSocket = accept(sock, (struct sockaddr *)&clientAddress, &clal);
        pthread_create(&threads[currentConnections], 0, requestHandler, 0);
    }
    
    for (i = 0; i < currentConnections; i++) {
        pthread_join(threads[i], 0);
    }
}

int main(int argc, char** argv) {
    teleAllocServer(atoi(argv[1]), 1);
}