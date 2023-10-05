#ifndef P0_LISTAFICHEROS_H
#define P0_LISTAFICHEROS_H

#include "types.h"
#include <stdbool.h>
#define LFNULL NULL

typedef struct tNodeF* tPosLF;
struct tNodeF{
    tItemLF data;
    tPosLF next;
};
typedef tPosLF tListF;


void createEmptyListF(tListF* L);
bool insertItemF(tItemLF d,tListF* L);
tItemLF getItemF(tPosLF p,tListF L);
void deleteAtPositionF(tPosLF p, tListF *L);
tPosLF findItemF(int descriptor, tListF L);
tPosLF firstF(tListF L);
void createDefaultListF(tListF* L);
void printListF(tListF L);
tPosLF nextF(tPosLF p,tListF L);
tPosLF previous(tPosLF p, tListF L);
void getFilePermFlags(tItemLF* item, int fd, int mode, bool isRDONLY);

#endif
