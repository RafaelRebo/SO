#ifndef SO
#define SO
#define MAX 100
#define MAXF 300
#define MAXM 60
#include "includes.h"
#include <stdbool.h>
#include <time.h>

typedef char comando[MAX];
typedef comando tComando;
typedef char filename[MAXF];
typedef filename tFilename;
typedef char mode[MAXM];
typedef mode tMode;
typedef enum allocType{
    Tmalloc,
    Tshared,
    Tmmap,
    Tall
}allocType;

typedef allocType tAlloctype;
typedef tComando tItemL;

typedef struct tItemLF{
    int descriptor;
    tFilename filename;
    tMode mode;
}tItemLF;

typedef struct tItemLM{
    void* memdir;
    int size;
    struct tm time;
    tAlloctype type;
    int sharedKey;
    tFilename mappedFilename;
    int mappedFD;
}tItemLM;

typedef struct parametros{
    bool longComand, linkComand, accComand, recaComand, recbComand, hidComand;
}tParametros;

#endif