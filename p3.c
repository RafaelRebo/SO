#include "includes.h"



char* getUserFromUID(uid_t uid){
    struct passwd *user;
    if ((user = getpwuid(uid)) == NULL) {
        perror("Error al obtener nombre de usuario");
        return "";
    }
    else return user->pw_name;
}

uid_t getUIDfromUser(const char* user){
    struct passwd *user_info;
    if((user_info=getpwnam(user))==NULL){
        perror("Usuario no encontrado");
        return -1;
    }
    return user_info->pw_uid;
}

void printDefault(){
    uid_t realUID=getuid();
    uid_t effectiveUID=geteuid();
    printf("Credencial real: %d (%s)\n",realUID, getUserFromUID(realUID));
    printf("Credencial efectiva: %d (%s)",effectiveUID, getUserFromUID(effectiveUID));
}

void Cmd_uid(char* trozos[]){
    if(trozos[1]==NULL || strcmp(trozos[1],"-get")==0 || strcmp(trozos[1],"-set")!=0){
        printDefault();
    }
    else if(strcmp(trozos[1],"-set")==0){
        if(trozos[2]==NULL){
            printDefault();
        }
        else if(strcmp(trozos[2],"-l")==0){
            if(trozos[3]==NULL){
                printDefault();
            }
            else{
                uid_t newUID=getUIDfromUser(trozos[3]);
                if (seteuid(newUID) == -1) {
                    perror("Error al establecer el UID");
                    return;
                }
            }
        }
        else if(trozos[2][0]>='0'&&trozos[2][0]<='9'){
            uid_t newUID=stringToInt(trozos[2]);
            if (seteuid(newUID) == -1) {
                perror("Error al establecer el UID");
                return;
            }
        }
        else return;
    }
}

int BuscarVariableEnvp (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/{
    int pos=0;
    char aux[MAXVAR];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

int BuscarVariableEnviron (char *var) {
    int pos = 0;
    char aux[MAXVAR];

    strcpy(aux, var);

    while (environ[pos] != NULL) {
        if (!strncmp(environ[pos], aux, strlen(aux))) {
            return pos;
        } else {
            pos++;
        }
    }

    errno=ENOENT;
    return (-1);
}

void Cmd_showvar(char* trozos[], char *envp[]){
    int count=0;
    char aux[MAXVAR];
    if(trozos[1]==NULL){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //Se podria hacer con environ, no se hizo para tener mas similitud a la shell de referencia
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,envp[count],envp[count]);
            count++;
        }
    }
    else{
        int selVariable;
        selVariable=BuscarVariableEnviron(trozos[1]);
        if (envp[selVariable]!=NULL&&BuscarVariableEnvp(trozos[1],envp)!=-1) {
            strcpy(aux,getenv(trozos[1]));
            printf("Con arg3 main:\n");
            printf("   > %s(%p) @%p\n", envp[selVariable],envp[selVariable], &envp[selVariable]);
            printf("Con variable global environ:\n");
            printf("   > %s(%p) @%p\n", environ[selVariable],environ[selVariable], &environ[selVariable]);
            printf("Con llamada a sistema getenv:\n");
            printf("   > %s (%p)\n", aux, aux);
        }
        else if(selVariable!=-1){
            printf("Con variable global environ:\n");
            printf("   > %s (%p) @%p\n", environ[selVariable],environ[selVariable], &environ[selVariable]);
            printf("Con llamada a sistema getenv:\n");
            printf("   > %s (%p)\n", aux, aux);
        }
    }
}

int findEqual(char* string){
    int pos=0;
    char* after=string;
    while(after[pos]!='\0'){
        if(after[pos]=='=') return pos+1;
        else pos++;
    }
    return (-1);
}

void Cmd_changevar(char* trozos[], char *envp[]){
    if(trozos[1]==NULL||(strcmp(trozos[1],"-a")!=0&&strcmp(trozos[1],"-e")!=0&&
                         strcmp(trozos[1],"-p")!=0)||trozos[2]==NULL||trozos[3]==NULL){
        printf("Uso: changevar [-a|-e|-p] var valor");
    }
    else{
        char aux[MAXVAR];
        int selVariable,pos;
        selVariable=BuscarVariableEnvp(trozos[2],envp);
        if(strcmp(trozos[1],"-a")==0){
            if (selVariable!=-1) {
                pos = findEqual(envp[selVariable]);
                strcpy(&envp[selVariable][pos], trozos[3]);
                printf("Variable de entorno %s cambiada al valor %s", trozos[2], trozos[3]);
            }
            else perror("No se ha encontrado la variable buscada");
        }
        else if(strcmp(trozos[1],"-e")==0){
            if (selVariable!=-1) {
                pos = findEqual(environ[selVariable]);
                strcpy(&environ[selVariable][pos], trozos[3]);
                printf("Variable de entorno %s cambiada al valor %s", trozos[2], trozos[3]);
            }
            else perror("No se ha encontrado la variable buscada");
        }
        else{
            strcpy(aux,trozos[2]);
            strcat(aux, "=");
            printf("%s",aux);
            if(selVariable==-1){ //Variable no creada
                strcat(aux,trozos[3]);
                if(putenv(aux)!=0) perror("No se ha podido crear la variable deseada");
                else printf("Variable de entorno %s creada",aux);
            }
            else{ //Variable ya existente
                strcpy(aux,trozos[2]);
                strcat(aux, "=");
                strcat(aux,trozos[3]);
                if(putenv(aux)!=0) perror("No se ha podido modificar la variable deseada");
                else printf("Variable de entorno %s cambiada al valor %s",trozos[2],trozos[3]);
            }
        }
    }
}

void Cmd_subsvar(char *trozos[], char *envp[]) {
    char error[500];
    if (trozos[1] == NULL || (strcmp(trozos[1], "-a") != 0 && strcmp(trozos[1], "-e") != 0)
        || trozos[2] == NULL || trozos[3] == NULL || trozos[4] == NULL) {
        printf("Uso: subsvar [-a|-e] var1 var2 valor");
    } else {
        char aux[MAXVAR];
        int selVariable;

        strcpy(aux, trozos[3]);
        strcat(aux, "=");
        strcat(aux, trozos[4]);
        if (strcmp(trozos[1], "-a") == 0) {
            selVariable = BuscarVariableEnvp(trozos[2], envp);
            if (selVariable != -1) {
                if (BuscarVariableEnvp(trozos[3], envp) != -1) {
                    sprintf(error, "Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                    errno = EEXIST;
                    perror(error);
                    return;
                }
                strcpy(envp[selVariable], aux);
                printf("Sustituida variable de entorno %s con %s=%s", trozos[2], trozos[3], trozos[4]);
            } else {
                sprintf(error, "Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                perror(error);
            }
        }
        else {
            selVariable = BuscarVariableEnviron(trozos[2]);
            if (selVariable != -1) {
                if (BuscarVariableEnviron(trozos[3]) != -1) {
                    sprintf(error, "Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                    errno = EEXIST;
                    perror(error);
                    return;
                }
                strcpy(environ[selVariable], aux);
                printf("Sustituida variable de entorno %s con %s=%s", trozos[2], trozos[3], trozos[4]);
            } else {
                sprintf(error, "Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                perror(error);
            }
        }
    }
}

void Cmd_showenv(char *trozos[], char *envp[]){
    int count=0;
    if(trozos[1]==NULL){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //Se podria hacer con environ, no se hizo para tener mas similitud a la shell de referencia
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,envp[count],envp[count]);
            count++;
        }
    }
    else if(strcmp(trozos[1],"-environ")==0){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //Se podria hacer con environ, no se hizo para tener mas similitud a la shell de referencia
        while (environ[count] != NULL) {
            printf(" > %p -> environ[%d]=(%p)  %s\n" ,&environ[count],count,environ[count],environ[count]);
            count++;
        }
    }
    else if(strcmp(trozos[1],"-addr")==0){
        printf("environ: %p (almacenado en %p)\n",&environ[0],&environ);
        printf("main arg3: %p (almacenado en %p)\n",&envp[0],&envp);
    }
    else{
        printf("Uso: showenv [-environ|-addr]");
    }
}

void Cmd_fork (char *trozos[],tListP *procL){
    pid_t pid;

    if ((pid=fork())==0){
        deleteListP(procL);
        printf ("ejecutando proceso %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}

char* cortarCadena(const char* cadena){
    char* returnedString = malloc(MAX * sizeof(char));
    for (int i =0; cadena[i]!='\0' && cadena[i]!='=';i++){
        returnedString[i]=cadena[i];
    }
    return returnedString;
}

int esVariable(char* command,  char *envp[]){
    char temp[MAX];
    for(int i = 0; envp[i]!=NULL; i++){
        strcpy(temp, cortarCadena(envp[i]));
        if(strcmp(command, temp)==0)
            return i;
    }
    return -1;
}

void exec (char* trozos[], char *envp[]){   //mirar como hacer para las variables (no se como funcionan en la de referencia)
    int i=1, pos;
    char* vars[20];
    if (trozos[1]==NULL){
        errno=EFAULT;
        perror("Imposible ejecutar");
        return;
    }
    pos = esVariable(trozos[i], envp);
    for (i = 1; pos!=-1; i++){
        vars[i-1]=envp[pos];
        pos = esVariable(trozos[i], envp);
        if(pos==-1) i--;
    }
    vars[i - 1] = NULL;
    printf("%d",i);
    if(i==1){
        if(execvp(trozos[i], &trozos[i])==-1)
            perror("Imposible ejecutar");
    }
    else if(execvpe(trozos[i], &trozos[i], vars)==-1)
        perror("Imposible ejecutar");
}

int ValorSenal(char * sen){
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal;
    return -1;
}


char *NombreSenal(int sen){			/* para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}


procStatus updateItems(tItemLP proc,tListP* Lproc, int* signal){
    int newStatus;
    tPosLP pos;
    procStatus updatedStatus;

    pid_t pid = waitpid(proc.pid, &newStatus, WNOHANG| WUNTRACED|WCONTINUED);

    *signal=newStatus;

    if(pid==0||WIFCONTINUED(newStatus)){
        updatedStatus=ACTIVE;
    }

    else if (pid > 0){
        if(WIFSTOPPED(newStatus)){
            updatedStatus=STOPPED;
            *signal=WSTOPSIG(newStatus);
        }
        else if(WIFEXITED(newStatus)) {
            updatedStatus=FINISHED;
            *signal= WTERMSIG(newStatus);
        }
        else if(WIFSIGNALED(newStatus)) {
            updatedStatus=SIGNALED;
        }
    }
    proc.status=updatedStatus;
    pos=findItemP(proc.pid,*Lproc);
    updateItemP(proc,pos,Lproc);
    return proc.status;
}

char* statusEnumToString(procStatus status){
    char* returnedString = malloc(9 * sizeof (char));
    char* v[]={"FINISHED", "STOPPED", "SIGNALED", "ACTIVE"};
    strcpy(returnedString, v[status]);
    return returnedString;
}

void jobs (tListP Lproc){
    tItemLP proc;
    char* status;
    int signal;
    for(tPosLP i = firstP(Lproc); i!=LPNULL; i= nextP(i, Lproc)){
        proc = getItemP(i, Lproc);
        proc.status=updateItems(proc,&Lproc,&signal);
        status=statusEnumToString(proc.status);
        printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%s) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
               proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status,
               NombreSenal(signal), proc.commandLine);
        free(status);
    }
}

void deljobs(char* trozos[],tListP* Lproc){
    tItemLP proc;
    char* status;
    int signal;
    if(trozos[1]==NULL){
        for(tPosLP i = firstP(*Lproc); i!=LPNULL; i= nextP(i, *Lproc)){
            proc = getItemP(i, *Lproc);
            proc.status=updateItems(proc,Lproc,&signal);
            status=statusEnumToString(proc.status);
            printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%03d) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                   proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status, 0, proc.commandLine);
            free(status);
        }
    }
    else if(strcmp(trozos[1],"-sig")==0){
        tPosLP i= firstP(*Lproc),temp;
        while(i!=LPNULL){
            proc=getItemP(i,*Lproc);
            if(proc.status==SIGNALED){
                temp=i;
                i=nextP(i,*Lproc);
                deleteAtPositionP(temp,Lproc);
            }
            else i=nextP(i,*Lproc);
        }
    }
    else if(strcmp(trozos[1],"-term")==0){
        tPosLP i= firstP(*Lproc),temp;
        while(i!=LPNULL){
            proc=getItemP(i,*Lproc);
            if(proc.status==FINISHED){
                temp=i;
                i=nextP(i,*Lproc);
                deleteAtPositionP(temp,Lproc);
            }
            else i=nextP(i,*Lproc);
        }
    }
}

void job (char* trozos[], tListP Lproc){
    tPosLP p;
    tItemLP proc;
    char* status;
    if(trozos[1]==NULL)
        jobs(Lproc);
    else if (!strcmp(trozos[1], "-fg")){
        if(trozos[2]==NULL)
            jobs(Lproc);
        else{
            p=findItemP(atoi(trozos[2]), Lproc);
            if(p==LPNULL)
                jobs(Lproc);
            else{
                proc = getItemP(p, Lproc);
                waitpid(proc.pid, NULL, 0);
            }
        }
    }
    else{
        p=findItemP(atoi(trozos[1]), Lproc);
        if(p==LPNULL)
            jobs(Lproc);
        else{
            proc = getItemP(p, Lproc);
            status= statusEnumToString(proc.status);
            printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%03d) %s", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                   proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec,
                   status, 0, proc.commandLine);
            free(status);
        }
    }
}

bool tieneAmpersand(char* trozos[]){
    int i;
    for(i = 0; trozos[i]!=NULL; i++){
        if (trozos[i][0]=='&'){
            trozos[i]=NULL;
            return true;
        }

    }
    return false;
}

char* trozosToString(char* trozos[]){
    char* returnedString = malloc(sizeof (tComando));
    strcpy(returnedString,trozos[0]);
    strcat(returnedString," ");
    for(int i = 1; trozos[i]!=NULL; i++){
        strcat(returnedString, trozos[i]);
        strcat(returnedString, " ");
    }
    return returnedString;
}


void runProcess(char* trozos[], tListP* Lproc){
    pid_t pid;
    bool bg= tieneAmpersand(trozos);
    tItemLP proc;
    pid=fork();
    switch (pid) {
        case -1:
            perror("Fork error");
            break;
        case 0:
            if(execvp(trozos[0], trozos)==-1){
                perror("Imposible ejecutar comando: ");
                exit(0);
            }
            break;
    }
    if(bg){
        time_t date = time(NULL);
        struct tm tm = *localtime(&date);
        proc.time=tm;
        proc.pid=pid;
        strcpy(proc.commandLine, trozosToString(trozos));
        proc.status=ACTIVE;//mirar si la inicializamos a esto o a nulo pa empezar o como
        if(!insertItemP(proc, LPNULL, Lproc))
            perror("No se pudo insertar");
    }
    else
        waitpid(pid, NULL, 0);
}