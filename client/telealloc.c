#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 

void teleWrite(struct TeleAllocation* target, char* bytes, size_t length) {
    char protocolInformation[40];
    int protocolLength = sprintf(protocolInformation, "%c%016llx%016llx", REQUEST_WRITE, target->memoryPointer, (long long) length);
    logDebug("Writing %s, header %s, length %ld", bytes, protocolInformation, length);
    send(target->targetSocket, protocolInformation, protocolLength, MSG_CONFIRM);
    send(target->targetSocket, bytes, length, MSG_CONFIRM);
}

int teleRead(struct TeleAllocation* target, char* bytes, size_t length) {
    char protocolInformation[40];
    int protocolLength = sprintf(protocolInformation, "%c%016llx%016llx", REQUEST_READ, target->memoryPointer, (long long) length);
    logDebug("Reading, header %s, length %ld", protocolInformation, length);
    send(target->targetSocket, protocolInformation, protocolLength, MSG_CONFIRM);
    return read(target->targetSocket, bytes, length);
}

struct TeleAllocation* teleAlloc(int target, size_t length) {
    char blah[40];
    struct TeleAllocation* targetAlloc;
    logDebug("Malloc: %ld", length);
    int protocolLength = sprintf(blah, "%c%016llx", REQUEST_MALLOC, (long long) length);
    send(target, blah, protocolLength, MSG_CONFIRM);
    
    targetAlloc = calloc(sizeof *targetAlloc, 1);
    targetAlloc->targetSocket = target;

    read(target, blah, 16);
    blah[16] = 0;
    logDebug("Got %p as malloc", blah);
    targetAlloc->memoryPointer = strtoll(blah, NULL, 16);

    return targetAlloc;
}

void teleRealloc(struct TeleAllocation* target, size_t newLength) {
    char blah[40];
    int protocolLength = sprintf(blah, "%c%016llx%016llx", REQUEST_REALLOC, target->memoryPointer, (long long) newLength);
    send(target->targetSocket, blah, protocolLength, MSG_CONFIRM);
    blah[16] = 0;
    read(target->targetSocket, blah, 16);
    target->memoryPointer = strtoll(blah, NULL, 16);
}

void *teleFree(struct TeleAllocation* target) {
    char protocolInformation[40];
    int protocolLength = sprintf(protocolInformation, "%c%016llx", REQUEST_FREE, target->memoryPointer);
    send(target->targetSocket, protocolInformation, protocolLength, MSG_DONTWAIT);
    
    target->memoryPointer = 0;
    target->targetSocket = 0;
    free(target);

    return 0;
}

void teleClose(int target) {
    char c = REQUEST_END;
    send(target, &c, 1, MSG_DONTWAIT);
}

int dial(char* ip, int port) {
    int targetSocket;
	struct sockaddr_in serverAddr;

	if ((targetSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		logError("Could not create socket");
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
		logError("Invalid IP address: %s", ip);
	}
	
	if (connect(targetSocket, (struct sockaddr *)&serverAddr, sizeof serverAddr) >= 0) {
		logInfo("Accepted connection to ip %s with port %d", ip, port);
	}
	else {
		logError("Could not accept connection to ip %s with port %d", ip, port);
	}

	return targetSocket;
}