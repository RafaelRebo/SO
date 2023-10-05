#ifndef SO
#define SO
#define MAX 100
#define MAXF 300
#define MAXM 60
#include <stdbool.h>

typedef char comando[MAX];
typedef comando tComando;
typedef char filename[MAXF];
typedef filename tFilename;
typedef char mode[MAXM];
typedef mode tMode;
typedef tComando tItemL;
typedef struct tItemLF{
    int descriptor;
    tFilename filename;
    tMode mode;
    int olddf;
}tItemLF;

#endif