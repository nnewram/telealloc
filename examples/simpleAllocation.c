#define LOGLEVEL 0
#include "../client/telealloc.h"

int main() {
    int sock = dial("127.0.0.1", 1338);
    struct TeleAllocation* myString = teleAlloc(sock, 69);
    char recvd[128];
    
    teleWrite(myString, "Bruh", 5);
    teleRead(myString, recvd, 5);
    printf("%s\n", recvd);

    teleRealloc(myString, 128);
    teleWrite(myString, "Why are you doing this shit, i wonder this every day and i still dont have an answare!", 87);
    teleRead(myString, recvd, 87);
    printf("%s\n", recvd);

    teleFree(myString);

    teleClose(sock);
}