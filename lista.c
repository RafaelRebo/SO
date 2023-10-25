
#include "includes.h"

void createEmptyList(tList* L){
    *L=LNULL;
}

bool isEmptyList(tList L){
    return (L==LNULL);
}


bool createNode(tPosL *p){
    *p=malloc(sizeof(struct tNode));
    return *p!=LNULL;
}

bool insertItem(tItemL d,tList* L){ //Inserta siempre los items al final de la lista
    tPosL q,r;
    if(!createNode(&q)) return false;
    else{
        strcpy(q->data,d);
        q->next=LNULL;
        if(*L==LNULL) *L=q;
        else{
            for(r=*L;r->next!=LNULL;r=r->next);
            r->next=q;
        }
        return true;
    }
}

tPosL first(tList L){
    return L;
}

tPosL next(tPosL p,tList L){
    return p->next;
}

void getItem(tPosL p,tList L,tComando* comando){
    strcpy(*comando,p->data);
}

void deleteList(tList *L){
    tPosL p;
    while (!isEmptyList(*L))
    {
        p = *L;
        *L = (*L)->next;
        free(p);
    }
}

void printList(tList L){ //Imprime la lista completa
    tPosL p;
    tComando comando;
    int index=0;
    for(p=first(L);p!=LNULL;p= next(p,L)){
        index++;
        getItem(p,L,&comando);
        printf("[%d] %s",index,comando);
    }
}

void printNElements(tList L,int n){ //Imprime solo los n primeros elementos de la lista
    tPosL p;
    tComando comando;
    int index=0,i=0;
    for (p =first(L);(p != LNULL)&&(i<n); p = next(p, L)) {
        index++;
        getItem(p, L, &comando);
        printf("[%d] %s", index, comando);
        i++;
    }
}