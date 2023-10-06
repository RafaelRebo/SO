/*
* AUTHOR 1: YAGO MANUEL SÁNCHEZ MOSCOSO LOGIN 1: yago.sanchezm@udc.es
* AUTHOR 2: RAFAEL REBOREDO BARREIRO LOGIN 2: rafael.reboredo@udc.es
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include "lista.h"
#include "listaficheros.h"

void imprimirPrompt();
void leerEntrada(char* comando);
bool procesarEntrada(tComando comando,tList* L,char* trozos[],tListF* F);
int TrocearCadena(char * cadena, char * trozos[]);
void procesarComando(char* trozos[],tList* L,tListF* F);
void authors(char* trozos[]);
void pid(char* trozos[]);
void chgdir(char* trozos[]);
void datetime(char* trozos[]);
void hist(char* trozos[],tList* L);
void command(char* trozos[],tList* L,tListF* F);
void Cmd_open(char * trozos[],tListF* F);
void Cmd_close (char* trozos[],tListF* F);
void Cmd_dup (char* trozos[],tListF* F);
void infosys(char* trozos[]);
char LetraTF (mode_t m);
void stats(char* trozos[]);
void Help(char* trozos[]);

int main() {
    bool terminado=false;
    tComando comando;
    char* trozos[10];
    tList L;
    tListF F;
    createDefaultListF(&F);
    createEmptyList(&L);
    while(!terminado){
        puts("");
        imprimirPrompt();
        leerEntrada(comando);
        terminado=procesarEntrada(comando,&L,trozos,&F);
    }
    exit(0);
}

void imprimirPrompt(){
    printf(">> ");
}

void leerEntrada(char *comando){
    fgets(comando,MAX,stdin);
}

bool procesarEntrada(tComando comando, tList* L,char* trozos[],tListF* F){
    tComando fullcommand;
    strcpy(fullcommand,comando);
    bool terminado=false;
    TrocearCadena(comando, trozos);
    if (strcmp(trozos[0],"command")!=0) { //Cualquier comando menos "command" se introduce en el historico
        insertItem(fullcommand, L);
    }
    if((strcmp(trozos[0],"quit")==0)||(strcmp(trozos[0],"exit")==0)||(strcmp(trozos[0],"bye")==0)){
        terminado=true;
        printf("Shell finalizado.\n");
    }
    else{
        procesarComando(trozos,L,F); //Funcion para seleccionar la funcion asociada al comando que se quiere ejecutar
    }
    return terminado;
}

void procesarComando(char* trozos[],tList* L,tListF* F){
    if(strcmp(trozos[0],"authors")==0){
        authors(trozos);
    }
    else if(strcmp(trozos[0],"pid")==0){
        pid(trozos);
    }
    else if(strcmp(trozos[0],"chdir")==0){
        chgdir(trozos);
    }
    else if((strcmp(trozos[0],"date")==0)||(strcmp(trozos[0],"time")==0)){
        datetime(trozos);
    }
    else if(strcmp(trozos[0],"hist")==0){
        hist(trozos,L);
    }
    else if(strcmp(trozos[0],"command")==0){
        command(trozos,L,F);
    }
    else if(strcmp(trozos[0],"open")==0){
        Cmd_open(trozos,F);
    }
    else if(strcmp(trozos[0],"close")==0){
        Cmd_close(trozos,F);
    }
    else if(strcmp(trozos[0],"dup")==0){
        Cmd_dup(trozos,F);
    }
    else if(strcmp(trozos[0],"listopen")==0){
        printListF(*F);
    }
    else if(strcmp(trozos[0],"infosys")==0){
        infosys(trozos);
    }
    else if(strcmp(trozos[0],"stat")==0){
        stats(trozos);
    }
    else if(strcmp(trozos[0],"help")==0){
        Help(trozos);
    }
    else{
        printf("Orden < %s > no encontrada. Ver 'help' para la lista de posibles comandos",trozos[0]);
    }
}

int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    //Hace una separacion del string cadena en funcion a los caracteres espacio, \n y \t
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    // Continuamos obteniendo trozos subsiguientes utilizando NULL como primer argumento(la funcion continua donde lo dejo la primera vez
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void authors(char* trozos[]){
    if (trozos[1]!=NULL) {
        if (strcmp(trozos[1], "-l") == 0) {
            printf("rafael.reboredo@udc.es & yago.sanchezm@udc.es");
        } else if (strcmp(trozos[1], "-n") == 0) {
            printf("Rafael Reboredo & Yago Manuel Sanchez");
        }
        else{
            printf("authors: opcion incorrecta < %s >. Ver 'help authors' para ayuda",trozos[1]);
        }
    }
    else{
        printf("Rafael Reboredo (rafael.reboredo@udc.es) & Yago Manuel Sanchez (yago.sanchezm@udc.es)");
    }

}

void pid(char* trozos[]){
    if(trozos[1]!=NULL){
        if(strcmp(trozos[1], "-p")==0){
            printf("Shell Parent PID: %d",getppid());
        }
        else{
            printf("pid: opcion incorrecta < %s >. Ver 'help pid' para ayuda",trozos[1]);
        }
    }
    else{
        printf("Shell PID: %d",getpid());
    }
}

void chgdir(char* trozos[]){
    int size=1000;
    char directorio[size];
    if(trozos[1]!=NULL){
        if(chdir(trozos[1])==0){    //si no hay errores al cambiar el directorio devuelve 0 en caso contrario -1
            printf("Directorio de trabajo cambiado a < %s >.",trozos[1]);
        }
        else{
            perror("No se pudo cambiar el directorio");
        }
    }
    else{
        getcwd(directorio,size); //Devuelve el directorio de trabajo actual
        printf("El directorio de trabajo actual es: < %s >.",directorio);
    }
}

void datetime(char* trozos[]){
    if(trozos[1]!=NULL){
        if(strcmp(trozos[0],"date")==0){
            printf("date: opcion incorrecta < %s >. Ver 'help date' para ayuda",trozos[1]);
        }
        else printf("time: opcion incorrecta < %s >. Ver 'help time' para ayuda",trozos[1]);
    }
    else{
        time_t date;
        //Guardamos en el struct tm predefinido en time.h la fecha y hora actuales para luego imprimirlas
        date=time(NULL);
        struct tm tm = *localtime(&date);
        if (strcmp(trozos[0],"date")==0) {
            printf("Fecha actual: %02d/%02d/%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        }
        else{
            printf("Hora actual: %02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
    }
}

void hist(char* trozos[],tList* L){
    int n;
    if(trozos[1]!=NULL){
        char numorletter=trozos[1][1];
        if(strcmp(trozos[1],"-c")==0){
            //Borra la lista con el argumento -c
            deleteList(L);
        }
        else if(numorletter<='0'||numorletter>'9'){
            printf("hist: opcion incorrecta < %s >. Ver 'help hist' para ayuda",trozos[1]);
        }
        else{
            //Convertimos el caracter que indica el numero de comandos a imprimir en int para usar la funcion printNElements
            n=atoi(&trozos[1][1]);
            printNElements(*L,n);
        }
    }
    else{
        //Sin argumentos imprime el historico
        printList(*L);
    }
}

void command(char* trozos[],tList* L,tListF* F){
    int n;
    tPosL p;
    tComando comando;
    if(trozos[1]!=NULL){
        char numorletter=trozos[1][0];
        if(numorletter<='0'||numorletter>'9'){
            printf("command: opcion incorrecta < %s >. Ver 'help command' para ayuda",trozos[1]);
        }
        else{
            int i=1;
            n=atoi(&trozos[1][0]); //El numero seleccionado como argumento
            if (!isEmptyList(*L)) {
                for (p = first(*L); (p != LNULL) && (i < n); p = next(p, *L)) {
                    i++;
                }
                if (p!=LNULL) { //Si se encuentra el comando buscado en la lista se vuelve a ejecutar
                    getItem(p, *L, &comando);
                    printf("Repitiendo comando:   [%d] %s\n", n, comando);
                    procesarEntrada(comando, L, trozos, F);
                }
                else{
                    printf("No se encuentra el comando %d en el historico.",n);
                }
            }
            else{
                printf("El historico de comandos esta vacio.");
            }
        }
    }
    else{
        printf("command: faltan argumentos. Ver 'help command' para ayuda");
    }
}

void Cmd_open (char * trozos[],tListF* F) {
    tItemLF item;
    bool isRDONLY=false;
    int i, df, mode = 0;
    if (trozos[1] == NULL) { //open sin argumentos
        printListF(*F);
        return;
    }
    for (i = 2; trozos[i] != NULL; i++) //Añade a la variable mode todos los modos seleccionados en los argumentos
        if (!strcmp(trozos[i], "cr")) mode |= O_CREAT; //64
        else if (!strcmp(trozos[i], "ex")) mode |= O_EXCL; //128
        else if (!strcmp(trozos[i], "ro")){isRDONLY=true; mode |= O_RDONLY;} //0
        else if (!strcmp(trozos[i], "wo")) mode |= O_WRONLY; //1
        else if (!strcmp(trozos[i], "rw")) mode |= O_RDWR; //2
        else if (!strcmp(trozos[i], "ap")) mode |= O_APPEND; //1024
        else if (!strcmp(trozos[i], "tr")) mode |= O_TRUNC; //512
        else break;

    if ((df = open(trozos[1], mode, 0777)) == -1) { //0777: representacion octal de los permisos asignados a la funcion open
        perror("Imposible abrir fichero");
    }
    else {
        //Se crea una nueva entrada en la lista con el fichero
        printf("Anadida entrada %d a la tabla ficheros abiertos",df);
        item.descriptor=df;
        strcpy(item.filename,trozos[1]);
        item.olddf=df;
        getFilePermFlags(&item,df,mode,isRDONLY); //Consigue mediante fcntl los flags de apertura del fichero
        insertItemF(item,F);
    }
}

void Cmd_close (char* trozos[],tListF* F){
    tPosLF p;
    int df;
    tItemLF item,dup;
    if (trozos[1]==NULL || (df=atoi(trozos[1]))<0) { //close sin argumentos
        printListF(*F);
        return;
    }
    else if (trozos[1][0]>='0'&&trozos[1][0]<='9') {
        if(df!=1&&df!=0) { //Evita cerrar los descriptores de entrada y salida estandar para evitar cuelgues
            if (close(df) == -1) {
                perror("Imposible cerrar descriptor");
            }
            else {
                //Si se encuentra el fichero buscado en la lista se cierra
                printf("Cerrando fichero con entrada %d", df);
                p = findItemF(df, *F);
                item=getItemF(p,*F);
                if (p != LFNULL){
                    for(tPosLF q= firstF(*F);q!=LFNULL;q= nextF(q,*F)){
                        dup=getItemF(q,*F);
                        if(dup.olddf==item.descriptor){
                            q= previous(q,*F);
                            deleteAtPositionF(nextF(q,*F),F);
                        }
                    }
                }
            }
        }
        else{
            printf("No se pueden cerrar los ficheros de entrada y salida estandar");
        }
    }
    else{
        printf("El argumento < %s > no es un descriptor. Por favor, especifique un descriptor valido.",trozos[1]);
    }
}

void Cmd_dup (char* trozos[],tListF* F){
    int df,newdf;
    tItemLF item,duplicado;
    tFilename filename;
    tMode mode;
    char aux[400];
    if (trozos[1]==NULL || (df=atoi(trozos[1]))<0) { //dup sin argumentos
        printListF(*F);
        return;
    }
    else {
        if (trozos[1][0] >= '0' && trozos[1][0] <= '9') {
            newdf = dup(df);
            if (newdf == -1) {
                perror("Imposible duplicar descriptor.");
            } else {
                //Busca el item a duplicar en la lista, y copia sus datos en un duplicado para luego insertarlo en la lista
                item = getItemF(findItemF(df, *F), *F);
                strcpy(filename, item.filename);
                strcpy(mode, item.mode);
                sprintf(aux, "dup %d (%s)", df, filename);
                duplicado.descriptor = newdf;
                duplicado.olddf=item.olddf;
                strcpy(duplicado.filename, aux);
                strcpy(duplicado.mode, mode);
                insertItemF(duplicado, F);
                printf("Duplicado fichero con descriptor %d",df);
            }
        }
        else{
            printf("El argumento < %s > no es un descriptor. Por favor, especifique un descriptor valido.",trozos[1]);
        }
    }
}

void infosys(char* trozos[]){
    if (trozos[1]==NULL) {
        //Guarda los datos del sistema en el struct predefinido sys/utsname.h para luego imprimirlos por pantalla
        struct utsname info;
        uname(&info);
        printf("Nombre del sistema                - %s \n", info.sysname);
        printf("Identificador de maquina          - %s \n", info.nodename);
        printf("Version del sistema operativo     - %s \n", info.release);
        printf("Nombre del sistema operativo      - %s \n", info.version);
        printf("Identificador del hardware        - %s \n", info.machine);
    }
    else {
        printf("infosys: opcion incorrecta < %s >. Ver 'help infosys' para ayuda",trozos[1]);
    }
}

char LetraTF (mode_t m){
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

void stats(char* trozos[]){
    bool isLong=false, isLink=false, isAcc=false;
    struct stat *buf = NULL;
    struct passwd pwent;
    struct passwd *pwentp;
    buf=malloc(sizeof(struct stat));
    int size=400, dirIndex;
    char directorio[size];
    char date[20];
    char namebuffer[100];
    if(trozos[1]==NULL){
        getcwd(directorio,size); //Devuelve el directorio de trabajo actual
        printf("%s",directorio);
    }
        //STAT sin args: Tamaño y nombre
        //STAT long: Fecha, 1, inodo, propietario, grupo, permisos, tamaño, nombre
        //STAT
    else{
        for(dirIndex=1;(strcmp(trozos[dirIndex],"-long")==0)||(strcmp(trozos[dirIndex],"-link")==0)||(strcmp(trozos[dirIndex],"-acc")==0);dirIndex++){
            if(strcmp(trozos[dirIndex],"-long")==0) isLong=true;
            else if(strcmp(trozos[dirIndex],"-link")==0) isLink=true;
            else if(strcmp(trozos[dirIndex],"-acc")==0) isAcc=true;
        }
        for(int j=dirIndex;trozos[j]!=NULL;j++) {
            lstat(trozos[j],buf);
            if(isLong){
                strftime(date, sizeof(date), "%d/%m/%y - %H:%M", localtime(&(buf->st_ctime)));
                getpwuid_r(buf->st_uid, &pwent, namebuffer, sizeof(namebuffer), &pwentp);
                if(isLink){
                    if(isAcc){ //SI LONG SI LINK SI ACC
                        printf("\t%s (%ld) %s %u %ld bytes %s",date,buf->st_ino,pwent.pw_name,buf->st_gid,buf->st_size,trozos[j]);
                    }
                    else{ //SI LONG SI LINK NO ACC
                        printf("\t%ld bytes %s",buf->st_size,trozos[j]);
                    }
                }
                else{
                    if(isAcc){ //SI LONG NO LINK SI ACC
                        printf("\t%ld bytes %s",buf->st_size,trozos[j]);
                    }
                    else{ //SI LONG NO LINK NO ACC
                        printf("\t%ld bytes %s",buf->st_size,trozos[j]);
                    }
                }
            }
            else{
                printf("\t%ld bytes %s",buf->st_size,trozos[j]);
            }
        }
    }
}


void Help(char* trozos[]) {
    if (trozos[1] != NULL) {
        printf("%s", trozos[1]);
        if (strcmp(trozos[1], "authors") == 0) {
            printf(" [-n|-l]\tMuestra los nombres y/o logins de los autores\n");
        } else if (strcmp(trozos[1], "pid") == 0) {
            printf(" [-p]\tMuestra el pid del shell o de su proceso padre\n");
        } else if (strcmp(trozos[1], "chdir") == 0) {
            printf(" [dir]\tCambia (o muestra) el directorio actual del shell\n");
        } else if (strcmp(trozos[1], "date") == 0) {
            printf(" \tMuestra la fecha actual\n");
        } else if (strcmp(trozos[1], "time") == 0) {
            printf(" \tMuestra la hora actual\n");
        } else if (strcmp(trozos[1], "hist") == 0) {
            printf(" [-c|-N]\tMuestra (o borra) el historico de comandos\n"
                   "\t-N: muestra los N primeros\n"
                   "\t-c: borra el historico\n");
        } else if (strcmp(trozos[1], "command") == 0) {
            printf(" [N]\tRepite el comando N (del historico)\n");
        } else if (strcmp(trozos[1], "open") == 0 || strcmp(trozos[1], "listopen") == 0) {
            printf(" fich m1 m2...\tAbre el fichero fich. y lo anade a la lista de ficheros abiertos del shell\n"
                   "\t m1, m2..es el modo de apertura (or bit a bit de los siguientes).\n"
                   "\tcr: O_CREAT\tap: O_APPEND\n"
                   "\tex: O_EXCL \tro: O_RDONLY\n"
                   "\trw: O_RDWR \two: O_WRONLY\n"
                   "\ttr: O_TRUNC\n");
        } else if (strcmp(trozos[1], "close") == 0) {
            printf(" df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
        } else if (strcmp(trozos[1], "dup") == 0) {
            printf(" df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
        } else if (strcmp(trozos[1], "infosys") == 0) {
            printf(" \tMuestra informacion de la maquina donde corre el shell\n");
        } else if (strcmp(trozos[1], "help") == 0) {
            printf(" [cmd]\tMuestra ayuda sobre los comandos\n"
                   "\tcmd: info sobre el comando cmd\n");
        } else if (strcmp(trozos[1], "quit") == 0 || strcmp(trozos[1], "exit") == 0 || strcmp(trozos[1], "bye") == 0) {
            printf(" \tTermina la ejecucion del shell\n");
        } else {
            printf(" no encontrado");
        }

    } else {
        printf("authors [-n|-l] pid [-p] chdir [dir] date time hist [-c|-N] command [N]"
               " open [filepath flags] close [df] dup [df] listopen infosys help [cmd] quit exit bye\n");
    }
}
