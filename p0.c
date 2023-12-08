/*
* AUTHOR 1: YAGO MANUEL SÁNCHEZ MOSCOSO LOGIN 1: yago.sanchezm@udc.es
* AUTHOR 2: RAFAEL REBOREDO BARREIRO LOGIN 2: rafael.reboredo@udc.es
*/

#include "includes.h"

int main(int argc, char *argv[], char *envp[]) {
    bool terminado = false;
    tComando comando;
    char *trozos[20];
    tList L;
    tListF F;
    tListLM memL;
    tListP procL;
    createDefaultListF(&F);
    createEmptyList(&L);
    createEmptyListM(&memL);
    createEmptyListP(&procL);
    while (!terminado) {
        puts("");
        imprimirPrompt();
        leerEntrada(comando);
        if (strcmp(comando,"\n")!=0) {
            terminado = procesarEntrada(comando, &L, trozos, &F, &memL, &procL, envp);
        }
    }
    deleteList(&L);
    deleteListF(&F);
    deleteListM(&memL);
    deleteListP(&procL);
    exit(0);
}

void authors(char *trozos[]) {
    if (trozos[1] != NULL) {
        if (strcmp(trozos[1], "-l") == 0) {
            printf("rafael.reboredo@udc.es & yago.sanchezm@udc.es");
        } else if (strcmp(trozos[1], "-n") == 0) {
            printf("Rafael Reboredo & Yago Manuel Sanchez");
        } else {
            printf("authors: opcion incorrecta < %s >. Ver 'help authors' para ayuda", trozos[1]);
        }
    } else {
        printf("Rafael Reboredo (rafael.reboredo@udc.es) & Yago Manuel Sanchez (yago.sanchezm@udc.es)");
    }
}

void pid(char *trozos[]) {
    if (trozos[1] != NULL) {
        if (strcmp(trozos[1], "-p") == 0) {
            printf("Shell Parent PID: %d", getppid());
        } else {
            printf("pid: opcion incorrecta < %s >. Ver 'help pid' para ayuda", trozos[1]);
        }
    } else {
        printf("Shell PID: %d", getpid());
    }
}

void chgdir(char *trozos[]) {
    int size = 1000;
    char directorio[size];
    if (trozos[1] != NULL) {
        if (chdir(trozos[1]) == 0) {    //si no hay errores al cambiar el directorio devuelve 0 en caso contrario -1
            printf("Directorio de trabajo cambiado a < %s >.", trozos[1]);
        } else {
            perror("No se pudo cambiar el directorio");
        }
    } else {
        getcwd(directorio, size); //Devuelve el directorio de trabajo actual
        printf("El directorio de trabajo actual es: < %s >.", directorio);
    }
}

void datetime(char *trozos[]) {
    if (trozos[1] != NULL) {
        if (strcmp(trozos[0], "date") == 0) {
            printf("date: opcion incorrecta < %s >. Ver 'help date' para ayuda", trozos[1]);
        } else printf("time: opcion incorrecta < %s >. Ver 'help time' para ayuda", trozos[1]);
    } else {
        time_t date;
        //Guardamos en el struct tm predefinido en time.h la fecha y hora actuales para luego imprimirlas
        date = time(NULL);
        struct tm tm = *localtime(&date);
        if (strcmp(trozos[0], "date") == 0) {
            printf("Fecha actual: %02d/%02d/%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        } else {
            printf("Hora actual: %02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
    }
}

void hist(char *trozos[], tList *L) {
    int n;
    if (trozos[1] != NULL) {
        char numorletter = trozos[1][1];
        if (strcmp(trozos[1], "-c") == 0) {
            //Borra la lista con el argumento -c
            deleteList(L);
        } else if (numorletter <= '0' || numorletter > '9') {
            printf("hist: opcion incorrecta < %s >. Ver 'help hist' para ayuda", trozos[1]);
        } else {
            //Convertimos el caracter que indica el numero de comandos a imprimir en int para usar la funcion printNElements
            n = atoi(&trozos[1][1]);
            printNElements(*L, n);
        }
    } else {
        //Sin argumentos imprime el historico
        printList(*L);
    }
}

void command(char *trozos[], tList *L, tListF *F, tListLM *memL, tListP *procL, char *envp[]) {
    int n;
    tPosL p;
    tComando comando;
    if (trozos[1] != NULL) {
        char numorletter = trozos[1][0];
        if (numorletter <= '0' || numorletter > '9') {
            printf("command: opcion incorrecta < %s >. Ver 'help command' para ayuda", trozos[1]);
        } else {
            int i = 1;
            n = atoi(&trozos[1][0]); //El numero seleccionado como argumento
            if (!isEmptyList(*L)) {
                for (p = first(*L); (p != LNULL) && (i < n); p = next(p, *L)) {
                    i++;
                }
                if (p != LNULL) { //Si se encuentra el comando buscado en la lista se vuelve a ejecutar
                    getItem(p, *L, &comando);
                    printf("Repitiendo comando:   [%d] %s\n", n, comando);
                    procesarEntrada(comando, L, trozos, F, memL, procL, envp);
                } else {
                    printf("command: No se encuentra el comando %d en el historico.", n);
                }
            } else {
                printf("command: El historico de comandos esta vacio.");
            }
        }
    } else {
        printf("command: faltan argumentos. Ver 'help command' para ayuda");
    }
}

void Cmd_open(char *trozos[], tListF *F) {
    tItemLF item;
    bool isRDONLY = false;
    int i, df, mode = 0;
    if (trozos[1] == NULL) { //open sin argumentos
        printListF(*F);
        return;
    }
    for (i = 2; trozos[i] != NULL; i++) //Añade a la variable mode todos los modos seleccionados en los argumentos
        if (!strcmp(trozos[i], "cr")) mode |= O_CREAT; //64
        else if (!strcmp(trozos[i], "ex")) mode |= O_EXCL; //128
        else if (!strcmp(trozos[i], "ro")) {
            isRDONLY = true;
            mode |= O_RDONLY;
        } //0
        else if (!strcmp(trozos[i], "wo")) mode |= O_WRONLY; //1
        else if (!strcmp(trozos[i], "rw")) mode |= O_RDWR; //2
        else if (!strcmp(trozos[i], "ap")) mode |= O_APPEND; //1024
        else if (!strcmp(trozos[i], "tr")) mode |= O_TRUNC; //512
        else break;

    if ((df = open(trozos[1], mode, 0777)) ==
        -1) { //0777: representacion octal de los permisos asignados a la funcion open
        perror("open: Imposible abrir fichero");
    } else {
        //Se crea una nueva entrada en la lista con el fichero
        printf("Anadida entrada %d a la tabla ficheros abiertos", df);
        item.descriptor = df;
        strcpy(item.filename, trozos[1]);
        getFilePermFlags(&item, df, mode, isRDONLY); //Consigue mediante fcntl los flags de apertura del fichero
        insertItemF(item, F);
    }
}

void Cmd_close(char *trozos[], tListF *F) {
    tPosLF p;
    int df;
    if (trozos[1] == NULL || (df = atoi(trozos[1])) < 0) { //close sin argumentos
        printListF(*F);
        return;
    } else if (trozos[1][0] >= '0' && trozos[1][0] <= '9') {
        if (df != 1 && df != 0) { //Evita cerrar los descriptores de entrada y salida estandar para evitar cuelgues
            if (close(df) == -1) {
                perror("close: Imposible cerrar descriptor");
            } else {
                //Si se encuentra el fichero buscado en la lista se cierra
                printf("Cerrando fichero con entrada %d", df);
                p = findItemF(df, *F);
                if (p != LFNULL) deleteAtPositionF(p, F);
            }
        } else {
            printf("close: No se pueden cerrar los ficheros de entrada y salida estandar");
        }
    } else {
        printf("close: El argumento < %s > no es un descriptor. Por favor, especifique un descriptor valido.", trozos[1]);
    }
}

void Cmd_dup(char *trozos[], tListF *F) {
    int df, newdf;
    tItemLF item, duplicado;
    tFilename filename;
    tMode mode;
    char aux[400];
    if (trozos[1] == NULL || (df = atoi(trozos[1])) < 0) { //dup sin argumentos
        printListF(*F);
        return;
    } else {
        if (trozos[1][0] >= '0' && trozos[1][0] <= '9') {
            newdf = dup(df);
            if (newdf == -1) {
                perror("dup: Imposible duplicar descriptor.");
            } else {
                //Busca el item a duplicar en la lista, y copia sus datos en un duplicado para luego insertarlo en la lista
                item = getItemF(findItemF(df, *F), *F);
                strcpy(filename, item.filename);
                strcpy(mode, item.mode);
                sprintf(aux, "dup %d (%s)", df, filename);
                duplicado.descriptor = newdf;
                strcpy(duplicado.filename, aux);
                strcpy(duplicado.mode, mode);
                insertItemF(duplicado, F);
                printf("Duplicado fichero con descriptor %d", df);
            }
        } else {
            printf("dup: El argumento < %s > no es un descriptor. Por favor, especifique un descriptor valido.", trozos[1]);
        }
    }
}

void infosys(char *trozos[]) {
    if (trozos[1] == NULL) {
        //Guarda los datos del sistema en el struct predefinido sys/utsname.h para luego imprimirlos por pantalla
        struct utsname info;
        uname(&info);
        printf("Nombre del sistema                - %s \n", info.sysname);
        printf("Identificador de maquina          - %s \n", info.nodename);
        printf("Version del sistema operativo     - %s \n", info.release);
        printf("Nombre del sistema operativo      - %s \n", info.version);
        printf("Identificador del hardware        - %s \n", info.machine);
    } else {
        printf("infosys: opcion incorrecta < %s >. Ver 'help infosys' para ayuda", trozos[1]);
    }
}

