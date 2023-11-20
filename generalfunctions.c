#include "includes.h"

void imprimirPrompt() {
    printf(">> ");
}

void leerEntrada(char *comando) {
    fgets(comando, MAX, stdin);
}

bool procesarEntrada(tComando comando, tList *L, char *trozos[], tListF *F, tListLM *memL) {
    tComando fullcommand;
    strcpy(fullcommand, comando);
    bool terminado = false;
    TrocearCadena(comando, trozos);
    if (strcmp(trozos[0], "command") != 0) { //Cualquier comando menos "command" se introduce en el historico
        insertItem(fullcommand, L);
    }
    if ((strcmp(trozos[0], "quit") == 0) || (strcmp(trozos[0], "exit") == 0) || (strcmp(trozos[0], "bye") == 0)) {
        terminado = true;
        printf("Shell finalizado.\n");
    } else {
        procesarComando(trozos, L, F, memL); //Funcion para seleccionar la funcion asociada al comando que se quiere ejecutar
    }
    return terminado;
}

void procesarComando(char *trozos[], tList *L, tListF *F, tListLM *memL) {
    if(!strcmp(trozos[1], "-?") || !strcmp(trozos[1], "-help")){
        trozos[1]=trozos[0];
        Help(&trozos[0]);
    }
    else if (strcmp(trozos[0], "authors") == 0) {
        authors(trozos);
    } else if (strcmp(trozos[0], "pid") == 0) {
        pid(trozos);
    } else if (strcmp(trozos[0], "chdir") == 0) {
        chgdir(trozos);
    } else if ((strcmp(trozos[0], "date") == 0) || (strcmp(trozos[0], "time") == 0)) {
        datetime(trozos);
    } else if (strcmp(trozos[0], "hist") == 0) {
        hist(trozos, L);
    } else if (strcmp(trozos[0], "command") == 0) {
        command(trozos, L, F, memL);
    } else if (strcmp(trozos[0], "open") == 0) {
        Cmd_open(trozos, F);
    } else if (strcmp(trozos[0], "close") == 0) {
        Cmd_close(trozos, F);
    } else if (strcmp(trozos[0], "dup") == 0) {
        Cmd_dup(trozos, F);
    } else if (strcmp(trozos[0], "listopen") == 0) {
        printListF(*F);
    } else if (strcmp(trozos[0], "infosys") == 0) {
        infosys(trozos);
    } else if (strcmp(trozos[0], "create") == 0) {
        create(trozos);
    } else if (strcmp(trozos[0], "stat") == 0) {
        stats(trozos);
    } else if (strcmp(trozos[0], "list") == 0) {
        list(trozos);
    } else if (strcmp(trozos[0], "delete") == 0) {
        delete(trozos);
    } else if (strcmp(trozos[0], "deltree") == 0) {
        deltree(trozos);
    } else if (strcmp(trozos[0], "malloc") == 0) {
        Cmd_malloc(trozos,memL);
    } else if (strcmp(trozos[0], "shared") == 0) {
        Cmd_shared(trozos, memL);
    } else if (strcmp(trozos[0], "mmap") == 0) {
        Cmd_mmap(trozos,memL);
    } else if (strcmp(trozos[0], "read") == 0) {
        Cmd_read(trozos);
    } else if (strcmp(trozos[0], "memdump") == 0) {
        Cmd_memdump(trozos);
    } else if (strcmp(trozos[0], "memfill") == 0) {
        Cmd_memfill(trozos);
    } else if (strcmp(trozos[0], "write") == 0) {
        Cmd_write(trozos);
    } else if (strcmp(trozos[0], "help") == 0) {
        Help(trozos);
    } else {
        printf("Orden < %s > no encontrada. Ver 'help' para la lista de posibles comandos", trozos[0]);
    }
}

int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    //Hace una separacion del string cadena en funcion a los caracteres espacio, \n y \t
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
        return 0;
    // Continuamos obteniendo trozos subsiguientes utilizando NULL como primer argumento (la funcion continua donde lo dejo la primera vez)
    while ((trozos[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

void Help(char *trozos[]) {
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
        } else if (strcmp(trozos[1], "create") == 0) {
            printf(" [-f] [name]\tCrea un directorio o un fichero (-f)");
        } else if (strcmp(trozos[1], "stat") == 0) {
            printf(" [-long][-link][-acc] name1 name2 ..\tlista ficheros;\n"
                   "\t-long: listado largo\n"

                   "\t-acc: acesstime\n"
                   "\t-link: si es enlace simbolico, el path contenido\n"
            );
        } else if (strcmp(trozos[1], "delete") == 0) {
            printf(" [name1 name2 ..]\tBorra ficheros o directorios vacios\n");
        } else if (strcmp(trozos[1], "deltree") == 0) {
            printf(" [name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente\n");
        } else if (strcmp(trozos[1], "list") == 0) {
            printf(" [-reca] [-recb] [-hid][-long][-link][-acc]\tn1 n2 ..\tlista contenidos de directorios\n"
                   "\t-hid: incluye los ficheros ocultos\n"
                   "\t-recb: recursivo (antes)\n"
                   "\t-reca: recursivo (despues)\n"
                   "\tresto parametros como stat\n");
        } else if (strcmp(trozos[1], "quit") == 0 || strcmp(trozos[1], "exit") == 0 || strcmp(trozos[1], "bye") == 0) {
            printf(" \tTermina la ejecucion del shell\n");
        } else {
            printf(" no encontrado");
        }

    } else {
        printf("authors [-n|-l] pid [-p] chdir [dir] date time hist [-c|-N] command [N]"
               " open [filepath flags] close [df] dup [df] listopen infosys create [-f] stat [-long|-link|-acc] list [-long|-link|-acc|-reca|-recb|-hid] delete deltree help [cmd] quit exit bye\n");
    }
}
