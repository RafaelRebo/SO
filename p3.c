#include "includes.h"

char* getUserFromUID(uid_t uid){
    //Dada una UID devuelve el nombre de usuario asociado
    struct passwd *user;
    if ((user = getpwuid(uid)) == NULL) {
        perror("uid: Error al obtener nombre de usuario");
        return "";
    }
    else return user->pw_name;
}

uid_t getUIDfromUser(const char* user){
    //Dado un nombre obtiene el UID del usuario asociado
    struct passwd *user_info;
    if((user_info=getpwnam(user))==NULL){
        perror("uid: Usuario no encontrado");
        return -1;
    }
    return user_info->pw_uid;
}

void printCredentials(){
    //Imprime los las credenciales reales y efectivas asociadas al proceso
    uid_t realUID=getuid();
    uid_t effectiveUID=geteuid();
    printf("Credencial real: %d (%s)\n",realUID, getUserFromUID(realUID));
    printf("Credencial efectiva: %d (%s)",effectiveUID, getUserFromUID(effectiveUID));
}

void Cmd_uid(char* trozos[]){
    //Caso uid, uid -get y comandos incompletos, imprime las credenciales
    if(trozos[1]==NULL || strcmp(trozos[1],"-get")==0 || strcmp(trozos[1],"-set")!=0){
        printCredentials();
    }
    else if(strcmp(trozos[1],"-set")==0){
        if(trozos[2]==NULL){
            printCredentials();
        }
        else if(strcmp(trozos[2],"-l")==0){
            if(trozos[3]==NULL){
                printCredentials();
            }
            else{
                //Caso uid -set -l login, se consigue el UID a partir del login proporcionado y se intenta establecer como nuevo EUID
                uid_t newUID=getUIDfromUser(trozos[3]);
                if (seteuid(newUID) == -1) {
                    perror("uid: Error al establecer el UID");
                    return;
                }
                printf("Establecido %s como nuevo usuario efectivo",trozos[3]);
            }
        }
        else if(trozos[2][0]>='0'&&trozos[2][0]<='9'){
            //Caso uid -set uid, se intenta establecer como nuevo EUID el uid proporcionado
            uid_t newUID=stringToInt(trozos[2]);
            if (seteuid(newUID) == -1) {
                perror("uid: Error al establecer el UID");
                return;
            }
            printf("Establecido UID=%s como nuevo usuario efectivo",trozos[2]);
        }
        else return;
    }
}

int BuscarVariableEnvp (char * var, char *e[]){

    /*busca una variable en el entorno que se le pasa como parámetro*/

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

    /*busca una variable en el entorno de la variable global environ*/

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
        //showvar sin argumentos imprime todas las variables de entorno
        printf("Imprimiendo todas las variables de entorno:\n\n");
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,envp[count],envp[count]);
            count++;
        }
    }
    else{
        int selVariable;
        selVariable=BuscarVariableEnviron(trozos[1]);
        //showvar VAR imprime los datos de la variable VAR
        if (envp[selVariable]!=NULL&&BuscarVariableEnvp(trozos[1],envp)!=-1) {
            //Caso de variable ya existente en el entorno
            strcpy(aux,getenv(trozos[1]));
            printf("Con arg3 main:\n");
            printf("   > %s(%p) @%p\n", envp[selVariable],envp[selVariable], &envp[selVariable]);
            printf("Con variable global environ:\n");
            printf("   > %s(%p) @%p\n",environ[selVariable],environ[selVariable], &environ[selVariable]);
            printf("Con llamada a sistema getenv:\n");
            printf("   > %s (%p)\n", aux, aux);
        }
        else if(selVariable!=-1){
            //Caso de variable creada con changevar -p
            printf("Con variable global environ:\n");
            printf("   > %s (%p) @%p\n", environ[selVariable],environ[selVariable], &environ[selVariable]);
            printf("Con llamada a sistema getenv:\n");
            printf("   > %s (%p)\n", aux, aux);
        }
    }
}

int findEqual(char* string){
    //Busca el caracter '=' en un string
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
        //Si la sintaxis es incorrecta se imprime una ayuda
        printf("Uso: changevar [-a|-e|-p] var valor");
    }
    else{
        char aux[MAXVAR];
        int selVariable,pos;
        selVariable=BuscarVariableEnvp(trozos[2],envp);
        if(strcmp(trozos[1],"-a")==0){
            if (selVariable!=-1) {
                //Se reemplaza a través del 3arg del main, envp, el valor de la variable deseada por el valor especificado
                pos = findEqual(envp[selVariable]);
                strcpy(&envp[selVariable][pos], trozos[3]);
                printf("Variable de entorno %s cambiada al valor %s", trozos[2], trozos[3]);
            }
            else perror("changevar: No se ha encontrado la variable buscada");
        }
        else if(strcmp(trozos[1],"-e")==0){
            if (selVariable!=-1) {
                //Se reemplaza a través de la variable global environ, el valor de la variable deseada por el valor especificado
                pos = findEqual(environ[selVariable]);
                strcpy(&environ[selVariable][pos], trozos[3]);
                printf("Variable de entorno %s cambiada al valor %s", trozos[2], trozos[3]);
            }
            else perror("changevar: No se ha encontrado la variable buscada");
        }
        else{
            strcpy(aux,trozos[2]);
            strcat(aux, "=");
            if(selVariable==-1){
                //Variable no creada. Con putenv podemos crear una nueva variable de entorno
                strcat(aux,trozos[3]);
                if(putenv(aux)!=0) perror("changevar: No se ha podido crear la variable deseada");
                else printf("Variable de entorno %s creada",aux);
            }
            else{
                //Variable ya existente, usamos putenv para cambiar su valor al especificado
                strcat(aux,trozos[2]);
                strcat(aux, "=");
                strcat(aux,trozos[3]);
                if(putenv(aux)!=0) perror("changevar: No se ha podido modificar la variable deseada");
                else printf("Variable de entorno %s cambiada al valor %s",trozos[2],trozos[3]);
            }
        }
    }
}

void Cmd_subsvar(char *trozos[], char *envp[]) {
    char error[500];
    if (trozos[1] == NULL || (strcmp(trozos[1], "-a") != 0 && strcmp(trozos[1], "-e") != 0)
        || trozos[2] == NULL || trozos[3] == NULL || trozos[4] == NULL) {
        //Si la sintaxis es incorrecta se imprime una ayuda
        printf("Uso: subsvar [-a|-e] var1 var2 valor");
    } else {
        char aux[MAXVAR];
        int selVariable;
        if (strcmp(trozos[1], "-a") == 0) {
            selVariable = BuscarVariableEnvp(trozos[2],envp);
            if (selVariable != -1) { //Si la variable se encuentra en el entorno...
                if (BuscarVariableEnvp(trozos[3], envp) != -1) { //Y la variable por la que se quiere sustituir no esta ya en el entorno...
                    sprintf(error, "subsvar: Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                    errno = EEXIST;
                    perror(error);
                    return;
                }
                //Se sustituye la variable por la especificada con el valor deseado
                strcpy(aux, trozos[3]);
                strcat(aux, "=");
                strcat(aux, trozos[4]);
                strcpy(envp[selVariable], aux);
                printf("Sustituida variable de entorno %s con %s=%s", trozos[2], trozos[3], trozos[4]);
            } else {
                fprintf(stderr, "subsvar: Imposible sustituir variable %s por %s: No such file or directory\n", trozos[2], trozos[3]);
            }
        }
        else {
            selVariable = BuscarVariableEnviron(trozos[2]);
            if (selVariable != -1||BuscarVariableEnviron(trozos[3]) != -1){ //Si la variable se encuentra en el entorno...
                if (BuscarVariableEnviron(trozos[2]) != -1&&BuscarVariableEnviron(trozos[3]) != -1) { //Y la variable por la que se quiere sustituir no esta ya en el entorno...
                    sprintf(error, "subsvar: Imposible sustituir variable %s por %s", trozos[2], trozos[3]);
                    errno = EEXIST;
                    perror(error);
                    return;
                }
                strcpy(aux, trozos[3]);
                strcat(aux, "=");
                strcat(aux, trozos[4]);
                //Se sustituye la variable por la especificada con el valor deseado
                if(selVariable!=-1) strcpy(environ[selVariable], aux);
                printf("Sustituida variable de entorno %s con %s=%s", trozos[2], trozos[3], trozos[4]);
            } else {
                fprintf(stderr, "subsvar: Imposible sustituir variable %s por %s: No such file or directory\n", trozos[2], trozos[3]);
            }
        }
    }
}

void Cmd_showenv(char *trozos[], char *envp[]){
    int count=0;
    if(trozos[1]==NULL){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //showenv sin argumentos imprime las variables de entorno accediendo por el 3 argumento del main
        while (envp[count] != NULL) {
            printf(" > %p -> main arg3[%d]=(%p)  %s\n" ,&envp[count],count,envp[count],envp[count]);
            count++;
        }
    }
    else if(strcmp(trozos[1],"-environ")==0){
        printf("Imprimiendo todas las variables de entorno:\n\n");
        //showenv -environ imprime las variables de entorno accediendo por la variable globar environ
        while (environ[count] != NULL) {
            printf(" > %p -> environ[%d]=(%p)  %s\n" ,&environ[count],count,environ[count],environ[count]);
            count++;
        }
    }
    else if(strcmp(trozos[1],"-addr")==0){
        //showenv -addr imprime las direcciones de la variable global environ y del array de variables de entorno del 3 argumento del main
        printf("environ: %p (almacenado en %p)\n",&environ[0],&environ);
        printf("main arg3: %p (almacenado en %p)\n",&envp[0],&envp);
    }
    else{
        //Si la sintaxis es incorrecta se imprime una ayuda
        printf("Uso: showenv [-environ|-addr]");
    }
}

void Cmd_fork (tListP *procL){
    pid_t pid;

    if ((pid=fork())==0){ //Crea un proceso hijo igual al padre
        deleteListP(procL);
        printf ("Ejecutando proceso %d\n", getpid());
    }
    else if (pid!=-1) //El padre espera a que el proceso hijo termine
        waitpid (pid,NULL,0);
}

//Funciones auxiliares para verificar que las posibles variables de entorno pasadas a exec existen y son correctas

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

//Funcion auxiliar que encuentra el argumento que contiene la prioridad deseada en un comando exec

int findPriorityArgument(char* trozos[], int* i){
    int prio=0;

    for(*i=0; trozos[*i]!=NULL; (*i)++) {
        if(trozos[*i][0]=='@'){
            prio=atoi(&trozos[*i][1]);
            trozos[*i]=NULL;
        }
    }

    return prio;
}

void exec (char* trozos[], char *envp[]){
    int i=1, pos, arrobaPos, prio;
    char* vars[20];
    //Busca y asigna correctamente el valor de la prioridad si existe
    prio=findPriorityArgument(trozos, &arrobaPos);
    if(arrobaPos!=0)
        trozos[arrobaPos]=NULL;

    if (trozos[1]==NULL){
        //exec sin argumentos provoca un error
        errno=EFAULT;
        perror("exec: Imposible ejecutar");
        return;
    }
    //Se escriben en el array vars todas las variables de entorno que se quieran en el entorno del programa a ejecutar con exec
    pos = esVariable(trozos[i], envp);
    for (i = 1; pos!=-1; i++){
        vars[i-1]=envp[pos];
        pos = esVariable(trozos[i], envp);
        if(pos==-1) i--;
    }
    vars[i - 1] = NULL;
    //Se asigna la prioridad especificada con setpriority
    setpriority(PRIO_PROCESS,getpid(), prio);
    if(i==1){
        //Sin variables de entorno
        if(execvp(trozos[i], &trozos[i])==-1)
            perror("exec: Imposible ejecutar");
    }
    else if(execvpe(trozos[i], &trozos[i], vars)==-1)
        //Con variables de entorno
        perror("exec: Imposible ejecutar");
}

char *NombreSenal(int sen){
    /* para sitios donde no hay sig2str*/
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}


procStatus updateItems(tItemLP proc,tListP* Lproc, int* signal){
    //Funcion auxiliar que se ejecuta cada vez que se imprime la lista de procesos
    //Actualiza el estado de cada proceso de la lista si es necesario y establece el valor de signal
    //al de la senal que ha senalado o terminado el proceso
    int newStatus;
    tPosLP pos;
    procStatus updatedStatus;

    pid_t pid = waitpid(proc.pid, &newStatus, WNOHANG|WUNTRACED|WCONTINUED);

    *signal=0;

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
            *signal=WTERMSIG(newStatus);
        }
        else if(WIFSIGNALED(newStatus)) {
            updatedStatus=SIGNALED;
            *signal=WTERMSIG(newStatus);
        }
    }
    proc.status=updatedStatus;
    pos=findItemP(proc.pid,*Lproc);
    updateItemP(proc,pos,Lproc);
    return proc.status;
}

char* statusEnumToString(procStatus status){
    //Convierte el enum que contiene la informacion del estado a string para imprimir
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
        //Para cada proceso, lo actualiza si no esta senalado o terminado, e imprime su informacion
        proc = getItemP(i, Lproc);
        if(proc.status!=SIGNALED&&proc.status!=FINISHED) proc.status=updateItems(proc,&Lproc,&signal);
        status=statusEnumToString(proc.status);
        if(proc.status==SIGNALED||proc.status==STOPPED){
            printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%s) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                   proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status,
                   NombreSenal(signal), proc.commandLine);
        }
        else{
            printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%03d) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                   proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status,
                   signal, proc.commandLine);
        }
        free(status);
    }
}

void deljobs(char* trozos[],tListP* Lproc){
    tItemLP proc;
    if(trozos[1]==NULL){
        //deljobs sin argumentos imprime la lista
        jobs(*Lproc);
    }
    else if(strcmp(trozos[1],"-sig")==0){
        //Recorre la lista eliminando los procesos cuyo estado es SIGNALED
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
        //Recorre la lista eliminando los procesos cuyo estado es FINISHED
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

void job (char* trozos[], tListP* Lproc){
    int newStatus;
    pid_t retPid;
    tPosLP p;
    tItemLP proc;
    char* status;
    int signal;
    if(trozos[1]==NULL)
        jobs(*Lproc); //Lista los procesos
    else if (!strcmp(trozos[1], "-fg")){
        if(trozos[2]==NULL)
            jobs(*Lproc); //Lista los procesos
        else{
            //Busca el proceso que se quiere pasar a primer plano, se espera a que termine y se indica la causa por la que lo hace
            p=findItemP(atoi(trozos[2]), *Lproc);
            if(p==LPNULL)
                jobs(*Lproc);
            else{
                proc = getItemP(p, *Lproc);
                deleteAtPositionP(p,Lproc);
                retPid=waitpid(proc.pid, NULL, 0);
                if (retPid > 0){
                    if(WIFEXITED(newStatus)){
                        printf("Proceso terminado normalmente. Valor devuelto %d",WTERMSIG(newStatus));
                    }
                    else if(WIFSIGNALED(newStatus)) {
                        printf("Proceso terminado por la senal %d",WTERMSIG(newStatus));
                    }
                }
            }
        }
    }
    else{
        //Busca el proceso deseado en la lista, y si existe, se actualiza su estado y se imprime su informacion
        p=findItemP(atoi(trozos[1]), *Lproc);
        if(p==LPNULL)
            jobs(*Lproc);
        else{
            proc = getItemP(p, *Lproc);
            if(proc.status!=SIGNALED&&proc.status!=FINISHED) proc.status=updateItems(proc,Lproc,&signal);
            status= statusEnumToString(proc.status);
            if(proc.status==SIGNALED||proc.status==STOPPED){
                printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%s) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                       proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status,
                       NombreSenal(signal), proc.commandLine);
            }
            else{
                printf("%d\t%s p=%d %02d/%02d/%02d %02d:%02d:%02d %s (%03d) %s\n", proc.pid, getUserFromUID(getuid()), getpriority(PRIO_PROCESS,proc.pid), proc.time.tm_year+1900,
                       proc.time.tm_mon+1, proc.time.tm_mday, proc.time.tm_hour, proc.time.tm_min, proc.time.tm_sec, status,
                       signal, proc.commandLine);
            }
            free(status);
        }
    }
}

bool tieneAmpersand(char* trozos[]){
    //Busca si un comando a ejecutar tiene '&', es decir, si se desea ejecutar en 2 plano
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
    //Convierte el array de string trozos en un string unico
    static char returnedString[MAX];
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
            //Si se crea el proceso hijo, se ejecuta en el el programa indicado, si es que existe
            if(execvp(trozos[0], trozos)==-1){
                perror("Imposible ejecutar comando: ");
                exit(0);
            }
            break;
        default:
            break;
    }
    if(bg){
        //Si el proceso se desea crear en segundo plano, se inserta en la lista de procesos y no se espera a que termine
        //para continuar la ejecucion de la shell padre
        time_t date = time(NULL);
        struct tm tm = *localtime(&date);
        proc.time=tm;
        proc.pid=pid;
        strcpy(proc.commandLine, trozosToString(trozos));
        proc.status=ACTIVE;
        if(!insertItemP(proc, LPNULL, Lproc))
            perror("No se pudo insertar");
    }
    else waitpid(pid, NULL, 0); //El proceso se quiere ejecutar en primer plano, la shell padre espera a que termine para continuar
}