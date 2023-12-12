
#ifndef P0_LISTAPROCESOS_H
#define P0_LISTAPROCESOS_H

#define LPNULL NULL


typedef struct tNodeLP* tPosLP;  //Ptr to tNode

struct tNodeLP {
    tItemLP data;
    tPosLP next;
};

typedef tPosLP tListP;

/* Function prototypes */

void createEmptyListP(tListP *L);;
bool insertItemP(tItemLP d, tPosLP p, tListP *L);
void updateItemP(tItemLP d, tPosLP p, tListP* L);
tPosLP findItemP(int pid, tListP L);
bool isEmptyListP(tListP L);
tItemLP getItemP(tPosLP p, tListP L);
tPosLP firstP(tListP L);
tPosLP lastP(tListP L);
tPosLP previousP(tPosLP p, tListP L);
tPosLP nextP(tPosLP p,tListP L);
void deleteAtPositionP(tPosLP p , tListP *L);
void deleteListP(tListP *L);
void vaciarLista(tListP *L);

#endif //P0_LISTAPROCESOS_H
