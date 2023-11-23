#include "includes.h"

void createEmptyListF(tListF* L){
    *L=LFNULL;
}
bool isEmptyListF(tListF L) { return (L == LFNULL); }

tPosLF findPosition(tListF L, const tItemLF d){
    tPosLF p, tmp;
    p = L;
    tmp = L;
    while ((p != LFNULL) && (p->data.descriptor < d.descriptor)){
        tmp = p;
        p = p->next;
    }
    return tmp;
}

bool createNodeF(tPosLF *p){
    *p=malloc(sizeof(struct tNodeF));
    return *p!=LFNULL;
}

bool insertItemF(tItemLF d, tListF *L){ //Inserta los items en la lista ordenados por DESCRIPTOR
    tPosLF q, p;
    if (!createNodeF(&q)){
        return false;
    }
    else{
        q->data = d;
        q->next = LFNULL;
        if (*L==LFNULL){
            *L = q;
        }
        else if (d.descriptor < (*L)->data.descriptor){
            q->next = *L;
            *L = q;
        }
        else{
            p = findPosition(*L, d);
            q->next = p->next;
            p->next = q;
        }
        return true;
    }
}

tPosLF firstF(tListF L){
    return L;
}

tPosLF nextF(tPosLF p,tListF L){
    return p->next;
}

tPosLF previous(tPosLF p, tListF L)
{
    tPosLF q;

    if (p == L)
    {
        return LFNULL;
    }
    else
    {
        for (q = L; q->next != p; q = q->next)
            ;
        return q;
    }
}

tItemLF getItemF(tPosLF p,tListF L){
    return p->data;
}

void deleteAtPositionF(tPosLF p, tListF *L){
    tPosLF q;
    if (p == *L){
        *L = (*L)->next;
    }
    else if (p->next == LFNULL){
        for (q = *L; q->next->next != LFNULL; q = q->next);
        q->next = LFNULL;
    }
    else{
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

void deleteListF(tListF *L){
    tPosLF p;
    while (!isEmptyListF(*L))
    {
        p = *L;
        *L = (*L)->next;
        free(p);
    }
}

tPosLF findItemF(int descriptor, tListF L){
    tPosLF p;
    for (p = L; (p != LFNULL) && (p->data.descriptor != descriptor); p = p->next);
    return p;
}

void printListF(tListF L){ //Imprime la lista completa
    tItemLF item;
    tPosLF p;
    for(p= firstF(L);p!=LFNULL;p= nextF(p,L)){
        item=getItemF(p,L);
        printf("Descriptor: %d -> %s %s\n",item.descriptor,item.filename,item.mode);
    }
}

void createDefaultListF(tListF* L){ //Crea los ficheros de entrada, salida y error estandar y los introduce en la lista
    tItemLF item;
    createEmptyListF(L);
    for (int i=0;i<=2;i++) {
        item.descriptor = i;
        if(i==0) strcpy(item.filename, "entrada estandar");
        else if(i==1) strcpy(item.filename, "salida estandar");
        else strcpy(item.filename, "error estandar");
        strcpy(item.mode,"O_RDWR");
        insertItemF(item,L);
    }
    if(tests_run_within_valgrind()){
        item.descriptor=3;
        strcpy(item.filename, "valgrind_report.txt");
        strcpy(item.mode,"O_RDWR");
        insertItemF(item,L);
    }
}


void getFilePermFlags(tItemLF* item,int fd, int mode, bool isRDONLY){
    int allflags = fcntl(fd, F_GETFL); //Devuelve un int que contiene la informacion de los flags de apertura del fichero
    int acc;
    tMode flag="";
    if (allflags == -1) {
        return;
    }
    else {
        acc = allflags & O_ACCMODE; //Se le aplica al valor anterior una mascara para poder filtrar los distintos modos de apertura y añadirlos
        if (acc == O_RDONLY && isRDONLY) {
            strcat(flag,"O_RDONLY ");
        }
        if (acc == O_WRONLY) {
            strcat(flag,"O_WRONLY ");
        }
        if (acc == O_RDWR) {
            strcat(flag,"O_RDWR ");
        }
        if (allflags & O_APPEND){
            strcat(flag,"O_APPEND ");
        }
        if(mode == (mode | O_CREAT)){
            strcat(flag,"O_CREAT ");
        }
        if(mode == (mode | O_EXCL)){
            strcat(flag,"O_EXCL ");
        }
        if(mode == (mode | O_TRUNC)){
            strcat(flag,"O_TRUNC ");
        }
    }
        strcpy(item->mode,flag);
}