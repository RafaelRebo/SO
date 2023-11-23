#include "includes.h"
#define TAMANO 2048

int vg1=2,vg2=4,vg3=6;

int vgn1,vgn2,vgn3;

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
    tAlloctype alloctype;
    strcpy(alloctype,"malloc");
    if(trozos[1]==NULL){
        printListM(*memL,alloctype);
    }
    else if(strcmp(trozos[1],"-free")==0){
        if(trozos[2]==NULL){
            printListM(*memL,alloctype);
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
            else printf("No se asignan bloques de 0 bytes");
        }
    }
    else{
        byteAmount= getByteAmount(trozos[1]);
        if (byteAmount>0) {
            time_t date = time(NULL);
            struct tm tm = *localtime(&date);
            int allocatedBytes = byteAmount;
            mallocItem.memdir = malloc(allocatedBytes);
            mallocItem.size = allocatedBytes;
            mallocItem.time = tm;
            strcpy(mallocItem.type, "malloc");
            mallocItem.mappedFD = -1;
            strcpy(mallocItem.mappedFilename, "");
            mallocItem.sharedKey = -1;
            printf("Asignados %d bytes en %p", allocatedBytes, mallocItem.memdir);
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
    tAlloctype alloctype;
    strcpy(alloctype,"shared");

    if (trozos[1]==NULL || trozos[2]==NULL || trozos[3]==NULL) {
        printListM(*M,alloctype);
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
    tAlloctype alloctype;
    strcpy(alloctype,"shared");
    if(trozos[2]==NULL){
        printListM(*M,alloctype);
    } else {
        key=getByteAmount(trozos[2]);
        p = findItemMS(key,*M, alloctype);
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

void sharedAttach(char *trozos[],tListLM* M){
    void * p;
    tItemLM item;
    int key=getByteAmount(trozos[1]);
    int id;
    struct shmid_ds s;
    if((id=shmget(key,0,0777))==-1){
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) key,strerror(errno));
        return;
    }
    if((p=shmat(id,NULL,0))==(void*)-1){
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) key,strerror(errno));
        return;
    }
    shmctl (id,IPC_STAT,&s);
    time_t date = time(NULL);
    struct tm tm = *localtime(&date);
    item.memdir = p;
    item.size = (int) s.shm_segsz;
    item.time = tm;
    strcpy(item.type, "shared");
    item.mappedFD = -1;
    strcpy(item.mappedFilename, "");
    item.sharedKey = key;
    printf("Memoria compartida de clave %d en %p",key,item.memdir);
    insertItemM(item, M);
}

void Cmd_shared(char *trozos[],tListLM* M){
    tAlloctype alloctype;
    strcpy(alloctype,"shared");
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
        else if(trozos[1][0]>='0'&&trozos[1][0]<='9'){
            sharedAttach(trozos,M);
        }
    }
    else{
        printListM(*M,alloctype);
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
    tAlloctype alloctype;
    strcpy(alloctype,"mmap");

    if (arg[0]==NULL)
    {printListM(*memL,alloctype); return;}
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
    tAlloctype alloctype;
    strcpy(alloctype,"mmap");
    if(trozos[1] != NULL && !strcmp(trozos[1], "-free")){
        if(trozos[2]==NULL)
            printListM(*memL, alloctype);
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
    void *p;
    unsigned long addr = strtoul(ar[1], NULL, 0);
    p = (void *) addr;
    size_t cont=-1;  /* -1 indica leer todo el fichero*/
    ssize_t n;
    if (ar[0]==NULL || ar[1]==NULL){
        printf ("faltan parametros\n");
        return;
    }

    if (ar[2]!=NULL)
        cont=(size_t) atoll(ar[2]);

    if ((n=LeerFichero(ar[0],p,cont))==-1)
        perror ("Imposible leer fichero");
    else
        printf ("leidos %lld bytes de %s en %p\n",(long long) n,ar[0],p);
}

void Cmd_read (char* trozos[]){
    CmdRead(&trozos[1]);
}

void printCharacter(size_t j, unsigned char *point){
    if ((point[j] >= 32 && point[j] <= 126)||point[j]==10){
        if(point[j]==10) printf("\\n");
        else printf(" %c ", point[j]);
    }
    else
        printf("   ");
}

void imprimirMemDumpHex(void *p, size_t len) {
    unsigned char *point = (unsigned char*)p;
    int cont=0;
    for (size_t j = 0; j <= len; j++){
        if((j%25!=0||j==0)&&j!=len)printCharacter(j,point);
        else{
            printf("\n");
            for (size_t i = j - cont; i < j; i++) {
                if (i == j - 1) {
                    printf("%02x ", point[i]);
                    printf("\n");
                } else printf("%02x ", point[i]);
            }
            cont = 0;
            if(j!=len) printCharacter(j,point);
        }
        cont++;
    }
}

void Cmd_memdump (char* trozos[]){
    void *p;
    size_t len=25;
    if(trozos[1]==NULL)
        return;
    if (trozos[2]!=NULL)
        len= atoi(trozos[2]);
    unsigned long addr = strtoul(trozos[1], NULL, 0);
    p = (void *) addr;
    printf("Volcando %ld bytes desde la direccion %p;\n\n", len, p);
    imprimirMemDumpHex(p, len);

}

void LlenarMemoria (void *p, size_t cont, unsigned char byte){
    unsigned char *arr=(unsigned char *) p;
    size_t i;
    for (i=0; i<cont;i++)
        arr[i]=byte;
}

void Cmd_memfill (char* trozos[]){
    void *p;
    int fillBytes = atoi(trozos[2]);
    char charBytes= 'A';
    unsigned long addr = strtoul(trozos[1], NULL, 0);
    p = (void *) addr;
    if (trozos[3]!=NULL) {
        if (trozos[3][0]==39&&trozos[3][2]==39) {
            charBytes = trozos[3][1];
        } else {
            charBytes=0;
        }
    }
    printf("Llenando %d bytes de memoria con el byte ' %c ' (%02x) a partir de la direccion %p", fillBytes, charBytes, charBytes, p);
    LlenarMemoria(p, fillBytes, charBytes);
}



ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite)
{
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;

    if ((df=open(f,flags,0777))==-1){
        perror("Imposible abrir fichero: ");
        return -1;
    }

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        perror("Imposible escribir fichero: ");
        return -1;
    }
    close (df);
    return n;
}

void Cmd_write(char* trozos[]){
    int overwrite = 0;
    size_t writtenBytes;

    if(trozos[1]!=NULL){
        if(strcmp(trozos[1],"-o")==0){
            if(trozos[2]==NULL || trozos[3]==NULL || trozos[4]==NULL){
                printf("faltan parametros");
                return;
            }
        }
        else if(trozos[2]==NULL || trozos[3]==NULL){
            printf("faltan parametros");
            return;
        }
    }
    else{
        printf("faltan parametros");
        return;
    }
    void *p;
    tFilename fich;
    int cont;

    strcpy(fich,trozos[1]);
    p = (void *) strtoul(trozos[2], NULL, 0);
    cont = atoi(trozos[3]);

    if (strcmp(trozos[1], "-o")==0){
        overwrite=1;
        strcpy(fich,trozos[2]);
        p = (void *) strtoul(trozos[3], NULL, 0);
        cont=atoi(trozos[4]);
    }

    writtenBytes=EscribirFichero(fich, p, cont, overwrite);

    if(writtenBytes!=-1){
        printf("Escritos %ld bytes en %s desde %p",writtenBytes,fich,p);
    }
}

void Do_MemPmap (void) /*sin argumentos*/{
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4]={"pmap",elpid,NULL};

    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){ /*proceso hijo*/
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
        if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");

        argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
        if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
        if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        exit(1);
    }
    waitpid (pid,NULL,0);
}

void Cmd_mem(char* trozos[],tListLM M){
    int var1=2,var2=3,var3=4;
    static int svar1=2,svar2=3,svar3=4;
    static int snvar1,snvar2,snvar3;
    tAlloctype alloctype;
    strcpy(alloctype,"all");
    if(trozos[1]!=NULL&&strcmp(trozos[1],"-all")!=0){
        if(strcmp(trozos[1],"-blocks")==0){
            printListM(M,alloctype);
        }
        else if(strcmp(trozos[1],"-vars")==0){
            printf("Variables locales: %19p, %17p, %17p\n",&var1,&var2,&var3);
            printf("Variables globales: %18p, %17p, %17p\n",&vg1,&vg2,&vg3);
            printf("Var(N.I.) globales: %18p, %17p, %17p\n",&vgn1,&vgn2,&vgn3);
            printf("Variables estaticas: %17p, %17p, %17p\n",&svar1,&svar2,&svar3);
            printf("Var(N.I.) estaticas: %17p, %17p, %17p\n",&snvar1,&snvar2,&snvar3);
        }
        else if(strcmp(trozos[1],"-funcs")==0){
            printf("Funciones de programa: %18p, %17p, %17p\n",&getByteAmount,&Cmd_shared,&printCharacter);
            printf("Funciones de liberia: %19p, %17p, %17p\n",&printf,&strcmp,&scanf);
        }
        else if(strcmp(trozos[1],"-pmap")==0){
            Do_MemPmap();
        }
    }
    else{
        printf("Variables locales: %19p, %17p, %17p\n",&var1,&var2,&var3);
        printf("Variables globales: %18p, %17p, %17p\n",&vg1,&vg2,&vg3);
        printf("Var(N.I.) globales: %18p, %17p, %17p\n",&vgn1,&vgn2,&vgn3);
        printf("Variables estaticas: %17p, %17p, %17p\n",&svar1,&svar2,&svar3);
        printf("Var(N.I.) estaticas: %17p, %17p, %17p\n",&snvar1,&snvar2,&snvar3);
        printf("\n");
        printf("Funciones de programa: %15p, %17p, %17p\n",&getByteAmount,&Cmd_shared,&printCharacter);
        printf("Funciones de liberia: %16p, %17p, %17p\n",&printf,&strcmp,&scanf);
        printf("\n");
        printListM(M,alloctype);
    }
}

void Recursiva (int n){
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

    if (n>0)
        Recursiva(n-1);
}

void Cmd_recurse(char* trozos[]){
    int n;
    if(trozos[1]!=NULL){
        n=getByteAmount(trozos[1]);
        Recursiva(n);
    }
}



