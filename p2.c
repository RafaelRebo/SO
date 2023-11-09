#include "includes.h"

void ImprimirListaMmap(tListLM **pM);

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
    int allocatedBytes;
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
            allocatedBytes = getByteAmount(trozos[2]);
            if(allocatedBytes<=0) {
                printf("No se asignan bloques de 0 bytes");
                return;
            }

            p = findItemMmalloc(allocatedBytes, *memL);
            if (p != NULL) {
                free(getItemM(p, *memL).memdir);
                deleteAtPositionM(p, memL);
            } else {
                printf("No hay bloque de ese tamano asignado con malloc");
            }

        }
    }
    else{
        allocatedBytes = getByteAmount(trozos[1]);
        if(allocatedBytes<=0) {
            printf("No se asignan bloques de 0 bytes");
            return;
        }
        time_t date = time(NULL);
        struct tm tm = *localtime(&date);
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


void * MapearFichero (char * fichero, int protection, tListLM* memL){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    tItemLM item;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;

    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;

    item.memdir = p;
    item.size=(int)s.st_size;
    strcpy(item.mappedFilename, fichero);
    item.mappedFD = df;
    time_t date = time(NULL);
    struct tm tm = *localtime(&date);
    item.time = tm;
    strcpy(item.type, "mmap");
    insertItemM(item, memL);

    return p;
}

void CmdMmap(char *arg[], tListLM* memL){
    char *perm;
    void *p;
    int protection=0;

    if (arg[0]==NULL)
    {printListM(*memL,"mmap"); return;}
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MapearFichero(arg[0],protection, memL))==NULL)
        perror ("Imposible mapear fichero");
    else
        printf ("fichero %s mapeado en %p\n", arg[0], p);
}

void Cmd_mmap(char* trozos[], tListLM* memL){
    tPosLM p;
    tItemLM item;
    if(trozos[1] != NULL && !strcmp(trozos[1], "-free")){
        if(trozos[2]==NULL)
            printListM(*memL, "mmap");
        else{

            p=findItemMmmap(trozos[2], *memL);

            if(p==LMNULL)
                printf("Fichero %s no mapeado", trozos[2]);
            else{
                item=getItemM(p, *memL);
                munmap(item.memdir, item.size);
                deleteAtPositionM(p, memL);
            }

        }
    }
    else
        CmdMmap(&trozos[1], memL);
}


