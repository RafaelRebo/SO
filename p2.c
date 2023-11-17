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
    int byteAmount;
    if(trozos[1]==NULL){
        printListM(*memL,"malloc");
    }
    else if(strcmp(trozos[1],"-free")==0){
        if(trozos[2]==NULL){
            printListM(*memL,"malloc");
        }
        else{
            byteAmount=getByteAmount(trozos[2]);
            if (byteAmount>0) {
                p = findItemMmalloc(byteAmount, *memL);
                if (p != NULL) {
                    mallocItem=getItemM(p, *memL);
                    printf("Liberados %d bytes de memoria asignada en %p",mallocItem.size,mallocItem.memdir);
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
        else printf("No se asignan bloques de 0 bytes");
    }
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam, tListLM* M){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    tItemLM item;

    if (tam) flags=flags | IPC_CREAT | IPC_EXCL; /*tam distito de 0 indica crear */
    if (clave==IPC_PRIVATE){
        /*no nos vale*/
        errno=EINVAL;
        return NULL;
    }
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    /** Guardar en la lista, p.e.  InsertarNodoShared (&L, p, s.shm_segsz, clave); */
    time_t date = time(NULL);
    struct tm tm = *localtime(&date);
    item.memdir = p;
    item.size = (int) tam;
    item.time = tm;
    strcpy(item.type, "shared");
    item.mappedFD = -1;
    strcpy(item.mappedFilename, "");
    item.sharedKey = clave;
    insertItemM(item, M);
    return (p);
}

void SharedCreate(char *trozos[],tListLM* M){
    key_t cl;
    size_t tam;
    void *p;

    if (trozos[1]==NULL || trozos[2]==NULL) {
        printListM(*M,"shared");
        return;
    }

    cl=(key_t) strtoul(trozos[2],NULL,10);
    tam=(size_t) strtoul(trozos[3],NULL,10);
    if (tam==0) {
        printf ("No se asignan bloques de 0 bytes\n");
        return;
    }
    if ((p=ObtenerMemoriaShmget(cl,tam,M))!=NULL)
        printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
    else
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void SharedFree(char *trozos[],tListLM* M){
    tItemLM item;
    tPosLM p;
    int key;
    if(trozos[2]==NULL){
        printListM(*M,"shared");
    } else {
        key=getByteAmount(trozos[2]);
        printf("%d",key);
        p = findItemMS(key,*M, "shared");
        if (p != NULL) {
            item = getItemM(p, *M);
            printf("Liberados %d bytes de memoria compartida en %p", item.size, item.memdir);
            shmdt(item.memdir);
            shmctl((unsigned long int) item.memdir,IPC_RMID,NULL);
            deleteAtPositionM(p, M);
        } else {
            printf("No hay bloque con esa clave asignado con shared");
        }
    }
}

void SharedDelkey (char *args[]){
    key_t clave;
    int id;
    char *key=args[2];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar id de memoria compartida\n");
}

void Cmd_shared(char *trozos[],tListLM* M){
    if(trozos[1]!=NULL){
        if(strcmp(trozos[1],"-create")==0){
            SharedCreate(trozos,M);
        }
        else if(strcmp(trozos[1],"-free")==0){
            SharedFree(trozos,M);
        }
        else if(strcmp(trozos[1],"-delkey")==0){
            SharedDelkey(trozos);
        }
    }
    else{
        printListM(*M,"shared");
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

ssize_t LeerFichero (char *f, void *p, size_t cont){
    struct stat s;
    ssize_t  n;
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

void CmdRead (char *ar[]){

    size_t cont=-1;  /* -1 indica leer todo el fichero*/
    ssize_t n;
    if (ar[0]==NULL || ar[1]==NULL){
        printf ("faltan parametros\n");
        return;
    }

    if (ar[2]!=NULL)
        cont=(size_t) atoll(ar[2]);

    if ((n=LeerFichero(ar[0],&ar[1],cont))==-1)
        perror ("Imposible leer fichero");
    else
        printf ("leidos %lld bytes de %s en %p\n",(long long) n,ar[0],&ar[1]);
}

void Cmd_read (char* trozos[]){
    CmdRead(&trozos[1]);
}

void imprimirMemDumpHex(void *p, size_t len) {
    unsigned char *point = (unsigned char*)p;
    for (size_t i = 0; i < len; i++) {
        if(i%25==0)
            printf("\n");
        printf("%02x ", point[i]);
    }
    printf("\n");
}

void Cmd_memdump (char* trozos[]){
    void *p;
    size_t len=25;
    if(trozos[1]==NULL)
        return;
    if (trozos[2]!=NULL)
        len= atoi(trozos[2]);
    unsigned long num = strtoul(trozos[1], NULL, 0);
    p = (void *) num;
    imprimirMemDumpHex(p, len);

}


