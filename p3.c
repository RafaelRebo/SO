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
    char **envp_aux_main = envp;
    char **envp_aux_environ = environ;
    if(trozos[1]==NULL){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //Se podria hacer con environ, no se hizo para tener mas similitud a la shell de referencia
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,(void*)*envp_aux_main,envp[count]);
            count++;
            envp_aux_main++;
        }
    }
    else if(strcmp(trozos[1],"-environ")==0){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //Se podria hacer con environ, no se hizo para tener mas similitud a la shell de referencia
        while (environ[count] != NULL) {
            printf(" > %p -> environ[%d]=(%p)  %s\n" ,&environ[count],count,(void*)*envp_aux_environ,environ[count]);
            count++;
            envp_aux_environ++;
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


procStatus updateItems(tItemLP proc){//falta hacer update de la prioridad
    int newStatus;

    pid_t pid = waitpid(proc.pid, &newStatus, WNOHANG);

    if(pid==0)
        return ACTIVE;
    else if (pid > 0){
        if(WIFEXITED(newStatus)) {
            return FINISHED;
        } else if(WIFSTOPPED(newStatus)) {
            return STOPPED;
        } else if(WIFSIGNALED(newStatus)) {
            return SIGNALED;
        }
    }

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
    for(tPosLP i = firstP(Lproc); i!=LPNULL; i= nextP(i, Lproc)){
        proc = getItemP(i, Lproc);
        printf("%d\t%s p=%d %d/%d/%d %d:%d:%d %u (%03d) %s\n", proc.pid, getUserFromUID(getuid()), proc.priority, proc.time.tm_year,
               proc.time.tm_mon, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, proc.status, 0, proc.commandLine);
    }
}

void job (char* trozos[], tListP Lproc){
    tPosLP p;
    tItemLP proc;
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
            printf("%d\t%s p=%d %d/%d/%d %d:%d:%d %s (%03d) %s", proc.pid, getUserFromUID(getuid()), proc.priority, proc.time.tm_year,
                   proc.time.tm_mon, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, proc.status, 0, proc.commandLine);
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
    for(int i = 0; trozos[i]!=NULL; i++){
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
            execvp(trozos[0], trozos);
            break;
    }
    if(bg){
        //proc.time=time(); obtener tiempo y guardarlo
        proc.pid=pid;
        strcpy(proc.commandLine, trozosToString(trozos));
        proc.priority=0;
        proc.status=ACTIVE;//mirar si la inicializamos a esto o a nulo pa empezar o como
        if(!insertItemP(proc, LPNULL, Lproc))
            perror("No se pudo insertar");
    }
    else
        waitpid(pid, NULL, 0);
}