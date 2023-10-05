

#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include "types.h"

#include <stdbool.h>
#define LNULL NULL


typedef struct tNode* tPosL;
struct tNode{
    tItemL data;
    tPosL next;
};
typedef tPosL tList;


void createEmptyList(tList* L);
bool insertItem(tItemL d,tList* L);
bool isEmptyList(tList L);
void getItem(tPosL p,tList L,tComando* comando);
void printList(tList L);
void printNElements(tList L,int n);
void deleteList(tList *L);
tPosL first(tList L);
tPosL next(tPosL p,tList L);


#endif

