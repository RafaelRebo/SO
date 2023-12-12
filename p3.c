#include "includes.h"

extern char **environ;

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

int BuscarVariable(char *var, char *envp[]){
    int pos=0;
    char aux[MAXVAR];

    strcpy (aux,var);

    while (envp[pos]!=NULL){
        if (!strncmp(envp[pos],aux,strlen(aux))){
            return (pos);
        }
        else pos++;
    }
    errno=ENOENT;   /*no hay tal variable*/
    perror("No se ha encontrado la variable buscada");
    return(-1);
}

void Cmd_showvar(char* trozos[], char *envp[]){
    int count=0;
    char aux[MAXVAR];
    char **envp_aux_main = envp;
    char **envp_aux_environ = environ;
    if(trozos[1]==NULL){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,(void*)*envp_aux_main,envp[count]);
            count++;
            envp_aux_main++;
        }
    }
    else{
        int selVariable;
        selVariable=BuscarVariable(trozos[1],envp);
        envp_aux_main=envp_aux_main+selVariable;
        envp_aux_environ=envp_aux_environ+selVariable;
        if (selVariable!=-1 && envp[selVariable]!=NULL) {
            strcpy(aux,getenv(trozos[1]));
            printf("Con arg3 main:\n");
            printf("   > %s(%p) @%p\n", *envp_aux_main, (void *)*envp_aux_main, envp_aux_main);
            printf("Con variable global environ:\n");
            printf("   > %s(%p) @%p\n", *envp_aux_environ, (void *)*envp_aux_environ, envp_aux_environ);
            printf("Con llamada a sistema getenv:\n");
            printf("   > %s (%p)\n", aux, &aux);
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
        selVariable=BuscarVariable(trozos[2],envp);
        if(strcmp(trozos[1],"-a")==0){
            pos=findEqual(envp[selVariable]);
            strcpy(&envp[selVariable][pos],trozos[3]);
            printf("Variable de entorno %s cambiada al valor %s",trozos[2],trozos[3]);
        }
        else if(strcmp(trozos[1],"-e")==0){
            pos=findEqual(environ[selVariable]);
            strcpy(&environ[selVariable][pos],trozos[3]);
            printf("Variable de entorno %s cambiada al valor %s",trozos[2],trozos[3]);
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
                printf("%s",aux);
                if(putenv(aux)!=0) perror("No se ha podido modificar la variable deseada");
                else printf("Variable de entorno %s cambiada al valor %s",trozos[2],trozos[3]);
            }
        }
    }
}

void Cmd_fork (char *tr[], tListP* Lproc){
    pid_t pid;

    if ((pid=fork())==0){

        vaciarLista(Lproc);
        printf("ejecutando proceso %d\n", getpid());
    }
    else if (pid!=-1)
        waitpid (pid,NULL,0);
}

bool esVariable(char* command,  char *envp[]){
    for(int i = 0; envp[i]!=NULL; i++){
        printf("%s ", envp[i]);
        if(!strcmp(command, envp[i]))
            return true;
    }
    return false;
}

void exec (char* trozos[]/*, char *envp[]*/){   //mirar como hacer para las variables (no se como funcionan en la de referencia)
    /*int i;
    char* vars[20];*/
    if (trozos[1]==NULL){
        printf("Imposible ejecutar: Bad address\n");
        return;
    }/*
    for (i = 2; esVariable(trozos[i], envp); i++){
        printf("si es");
        vars[i-2]=trozos[i];
    }
    vars[i-2]=NULL;

    if(execve(trozos[1], &trozos[i], vars)==-1)
        perror("Imposible ejecutar");
    */
    if(execvp(trozos[1], &trozos[2])==-1)
        perror("Imposible ejecutar");
}

void jobs (char* trozos[], tListP Lproc){
    tItemLP proc;
    for(tPosLP i = firstP(Lproc); i!=LPNULL; i= nextP(i, Lproc)){
        proc = getItemP(i, Lproc);
        printf("%d\t%s p=%d %d/%d/%d %d:%d:%d %u (%03d) %s\n", proc.pid, "calcular", proc.priority, proc.time.tm_year, proc.time.tm_mon, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, proc.status, 0, proc.commandLine);
    }
}

void job (char* trozos[], tListP Lproc){
    tPosLP p;
    tItemLP proc;
    if(trozos[1]==NULL)
        jobs(trozos, Lproc);
    else if (!strcmp(trozos[1], "-fg")){
        if(trozos[2]==NULL)
            jobs(trozos, Lproc);
        else{
            //hacer que un proceso pase a fg
        }
    }

    else{
        p=findItemP(atoi(trozos[1]), Lproc);
        if(p==LPNULL)
            jobs(trozos, Lproc);
        else{
            proc = getItemP(p, Lproc);

            printf("%d\t%s p=%d %d/%d/%d %d:%d:%d %u (%03d) %s", proc.pid, "calcular", proc.priority, proc.time.tm_year, proc.time.tm_mon, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, proc.status, 0, proc.commandLine);

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