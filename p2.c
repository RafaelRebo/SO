#include "includes.h"

int getByteAmount(const char string[]){
    char bytes[100]="";
    char letter;
    for(int i=0;string[i]>='0'&&string[i]<='9';i++){
        letter=string[i];
        bytes[i]=letter;
    }
    return atoi(bytes);
}

void Cmd_malloc(char* trozos[],tListLM* memL){
    tItemLM mallocItem;
    tPosLM p;
    if(trozos[1]==NULL){
        printListM(*memL,"malloc");
    }
    else if(strcmp(trozos[1],"-free")==0){
        if(trozos[2]==NULL){
            printListM(*memL,"malloc");
        }
        else{
            if (trozos[2]>0) {
                p = findItemM(getByteAmount(trozos[2]), *memL, "malloc");
                if (p != NULL) {
                    free(getItemM(p, *memL).memdir);
                    deleteAtPositionM(p, memL);
                } else {
                    printf("No hay bloque de ese tamano asignado con malloc");
                }
            }
        }
    }
    else{
        if (trozos[1]>0) {
            time_t date = time(NULL);
            struct tm tm = *localtime(&date);
            int allocatedBytes = getByteAmount(trozos[1]);
            mallocItem.memdir = malloc(allocatedBytes);
            mallocItem.size = allocatedBytes;
            mallocItem.time = tm;
            strcpy(mallocItem.type, "malloc");
            mallocItem.mappedFD = -1;
            strcpy(mallocItem.mappedFilename, "");
            mallocItem.sharedKey = -1;
            printf("Asignados %d bytes en %p", allocatedBytes, malloc(allocatedBytes));
            insertItemM(mallocItem, memL);
        }
    }
}