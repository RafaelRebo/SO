#ifndef P0_LISTAMEMORIA_H
#define P0_LISTAMEMORIA_H

#define LMNULL NULL


typedef struct tNodeM* tPosLM;
struct tNodeM{
    tItemLM data;
    tPosLM next;
};
typedef tPosLM tListLM;

void createEmptyListM(tListLM* M);
bool insertItemM(tItemLM d,tListLM* M);
bool isEmptyListM(tListLM M);
tItemLM getItemM(tPosLM p,tListLM M);
void deleteAtPositionM(tPosLM p, tListLM *M);
void deleteListM(tListLM *M);
tPosLM firstM(tListLM M);
tPosLM nextM(tPosLM p,tListLM M);
tPosLM findItemM(int bytes, tListLM M, tAlloctype alloctype);
tPosLM findItemMS(int bytes, tListLM M, tAlloctype alloctype);
void printListM(tListLM M,tAlloctype allocType);


#endif //P0_LISTAMEMORIA_H
