#ifndef __TELEALLOC_CLIENT__
#define __TELEALLOC_CLIENT__
#include <stdlib.h>

struct TeleAllocation;

int dial(char* ip, int port);

void teleWrite(struct TeleAllocation* target, char* bytes, size_t length);
int teleRead(struct TeleAllocation* target, char* bytes, size_t length);

struct TeleAllocation* teleAlloc(int target, size_t length);
void teleRealloc(struct TeleAllocation* target, size_t newLength);
void *teleFree(struct TeleAllocation* target);

void teleClose(int target);

enum LOG_LEVELS {SILENT=0, INFO, DEBUG, ERROR};
enum REQUESTS {REQUEST_READ=1, REQUEST_WRITE, REQUEST_MALLOC, REQUEST_REALLOC, REQUEST_FREE, REQUEST_END};

#define logInfo(fmt, ...) \
    do { \
        if (LOGLEVEL >= INFO) { \
            fprintf(stderr, fmt "\n", __VA_ARGS__); \
        } \
    } while(0)

#define logDebug(fmt, ...) \
    do { \
        if (LOGLEVEL >= DEBUG) { \
            fprintf(stderr, "Debug in file %s, line %d, function %s: " fmt "\n", __FILE__, __LINE__,  __func__, ##__VA_ARGS__); \
        } \
    } while (0)

#define logError(fmt, ...) \
    do { \
        if (LOGLEVEL != SILENT) { \
            fprintf(stderr, "Error in file %s, line %d, function %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
            exit(0); \
        } \
    } while(0)

#define LOGASSERT(statement, fmt, ...) \
    do { \
        if (statement) { \
            logError(fmt, __VA_ARGS__) \
        } \
    } while(0)

struct TeleAllocation {
    long long memoryPointer;
    int targetSocket;
};

#include "telealloc.c"
#endif