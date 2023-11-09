#include "includes.h"

void createEmptyListM(tListLM* M){
    *M=LMNULL;
}

bool isEmptyListM(tListLM M){
    return (M==LMNULL);
}


bool createNodeM(tPosLM *p){
    *p=malloc(sizeof(struct tNodeM));
    return *p!=LMNULL;
}

bool insertItemM(tItemLM d,tListLM* M){ //Inserta siempre los items al final de la lista
    tPosLM q,r;
    if(!createNodeM(&q)) return false;
    else{
        q->data=d;
        q->next=LMNULL;
        if(*M==LMNULL) *M=q;
        else{
            for(r=*M;r->next!=LMNULL;r=r->next);
            r->next=q;
        }
        return true;
    }
}

tPosLM firstM(tListLM M){
    return M;
}

tPosLM nextM(tPosLM p,tListLM M){
    return p->next;
}

tItemLM getItemM(tPosLM p,tListLM M){
    return p->data;
}

void getMonthName(int month, char* string){
    switch(month){
        case 0:
            strcpy(string,"Ene");
            break;
        case 1:
            strcpy(string,"Feb");
            break;
        case 2:
            strcpy(string,"Mar");
            break;
        case 3:
            strcpy(string,"Abr");
            break;
        case 4:
            strcpy(string,"May");
            break;
        case 5:
            strcpy(string,"Jun");
            break;
        case 6:
            strcpy(string,"Jul");
            break;
        case 7:
            strcpy(string,"Ago");
            break;
        case 8:
            strcpy(string,"Sep");
            break;
        case 9:
            strcpy(string,"Oct");
            break;
        case 10:
            strcpy(string,"Nov");
            break;
        case 11:
            strcpy(string,"Dic");
            break;
        default:
            break;
    }
}

void printListM(tListLM M, tAlloctype allocType){ //Imprime la lista completa
    tItemLM item;
    char string[4];
    printf("****** Lista de bloques asigandos %s para el proceso %d",allocType,getpid());
    for(tPosLM p=firstM(M); p!=LMNULL; p= nextM(p,M)){
        item=getItemM(p,M);
        if(!strcmp(item.type,allocType)){
            getMonthName(item.time.tm_mon,string);
            printf("\n\t%p\t\t%d\t %s %2d    %02d:%02d    %s",item.memdir,item.size,string,item.time.tm_mday,item.time.tm_hour,item.time.tm_min,item.type);
            if(!strcmp(item.type, "mmap"))
                printf("\t (descriptor %d)", item.mappedFD);
        }
    }
}

tPosLM findItemMmalloc(int bytes, tListLM M){
    tPosLM p;
    tItemLM item;
    if(isEmptyListM(M)) return LMNULL;
    p= firstM(M);
    do{
        item=getItemM(p,M);
        if(bytes==item.size&&!strcmp("malloc",item.type)){
            return p;
        }
        p=nextM(p,M);
    }
    while(p != LMNULL);
    return p;
}

tPosLM findItemMmmap(char* fileName, tListLM M){
    tPosLM p;
    tItemLM item;
    if(isEmptyListM(M)) return LMNULL;
    p= firstM(M);
    do{
        item=getItemM(p,M);
        if(!strcmp(fileName, item.mappedFilename)&&!strcmp("mmap",item.type)){
            return p;
        }
        p=nextM(p,M);
    }
    while(p != LMNULL);
    return p;
}

void deleteAtPositionM(tPosLM p, tListLM *M){
    tPosLM q;

    if (p == *M){
        *M = (*M)->next;
    }
    else if (p->next == LNULL){
        for (q = *M; q->next->next != LNULL; q = q->next);
        q->next = LNULL;
    }
    else{
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

void deleteListM(tListLM *M){
    tPosLM p;
    while (!isEmptyListM(*M))
    {
        p = *M;
        *M = (*M)->next;
        free(p);
    }
}



