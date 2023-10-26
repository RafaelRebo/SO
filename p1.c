
#include "includes.h"

char LetraTF(mode_t m) {
    switch (m & S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK:
            return 's'; /*socket */
        case S_IFLNK:
            return 'l'; /*symbolic link*/
        case S_IFREG:
            return '-'; /* fichero normal*/
        case S_IFBLK:
            return 'b'; /*block device*/
        case S_IFDIR:
            return 'd'; /*directorio */
        case S_IFCHR:
            return 'c'; /*char device*/
        case S_IFIFO:
            return 'p'; /*pipe*/
        default:
            return '?'; /*desconocido, no deberia aparecer*/
    }
}

char *ConvierteModo(mode_t m, char *permisos) {
    strcpy(permisos, "---------- ");

    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';    /*propietario*/
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';    /*grupo*/
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';    /*resto*/
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's';    /*setuid, setgid y stickybit*/
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';

    return permisos;
}

void inicializarParametros(tParametros *parametros){
    //Inicializa un struct con todos los parámetros disponibles de los comandos "stat" y "list" a falso
    parametros->longComand=false;
    parametros->linkComand=false;
    parametros->accComand = false;
    parametros->recaComand = false;
    parametros->recbComand = false;
    parametros->hidComand = false;
}

int leerParametros(char *trozos[], tParametros *parametros) {
    //Dado un comando, lee cuales de los 6 parámetros posibles de los comandos "stat" o "list" están en el comando del usuario
    int dirIndex = 0;
    inicializarParametros(parametros);
    bool continuar = true;

    do {
        dirIndex += 1;
        if(trozos[dirIndex]==NULL) continuar=false;
        else if (strcmp(trozos[dirIndex], "-long") == 0) parametros->longComand = true;
        else if (strcmp(trozos[dirIndex], "-link") == 0) parametros->linkComand = true;
        else if (strcmp(trozos[dirIndex], "-acc") == 0) parametros->accComand = true;
        else if (strcmp(trozos[dirIndex], "-reca") == 0){
            parametros->recaComand = true;
            parametros->recbComand= false;
        }
        else if (strcmp(trozos[dirIndex], "-recb") == 0){
            parametros->recbComand = true;
            parametros->recaComand = false;
        }
        else if (strcmp(trozos[dirIndex], "-hid") == 0) parametros->hidComand = true;
        else continuar = false;
    } while (continuar);
    return dirIndex; //Devuelve el primer índice de trozos que no es un parámetro del comando, sino que es algún tipo de fichero
}

void create(char *trozos[]) {
    char dir[1000];
    getcwd(dir, 1000);
    if (trozos[1] == NULL) {
        printf("%s", dir); //Si el comando no tiene parámetros se imprime el directorio actual
    }
    else if (strcmp(trozos[1], "-f") == 0) {
        if (trozos[2] == NULL) {
            printf("%s", dir);
        }
        else if (open(trozos[2], O_CREAT, 0777) == -1) perror("Imposible crear fichero"); //Si create -f fichero da error se muestra por pantalla
        else {
            //Si no da error, se informa de que el fichero se creó con éxito
            strcat(dir, "/");
            strcat(dir, trozos[2]);
            printf("Fichero %s creado satisfactoriamente en < %s >", trozos[2], dir);
        }
    }
    else {
        //Si el comando no tiene el parámetro -f se entiende que se quiere crear un directorio
        if (mkdir(trozos[1], 0777) == -1) perror("Imposible crear directorio"); //Si no se puede crear se muestra error
        else {
            //Si no da error, se informa de que el directorio se creó con éxito
            strcat(dir, "/");
            strcat(dir, trozos[1]);
            printf("Directorio creado en < %s >", dir);
        }
    }

}
void statOneFile(char *file, tParametros parametros) {
    time_t returnedTime;
    int size = 400;
    char date[20], permisos[20], directorioLink[size];
    struct stat buf;
    struct passwd *user;
    struct group *group;

    if (lstat(file, &buf) == -1) { //No se han podido volcar los datos del fichero
        perror("No ejecutado");
        return;
    }
    if (parametros.longComand) { //Si el comando incluye -long se deben mostrar mas datos
        ConvierteModo(buf.st_mode, permisos); //Función que a partir de la información del fichero devuelve un string con la información de los permisos
        if (parametros.accComand) returnedTime = buf.st_atime; //Usamos la hora de ultimo acceso si es -acc
        else returnedTime = buf.st_mtime; //Y si no la de última modificación
        strftime(date, sizeof(date), "%d/%m/%y - %H:%M", localtime(&(returnedTime))); //Guardamos la información de fecha y hora en date
        //Obtenemos la información de usuario y grupo
        if ((user = getpwuid(buf.st_uid)) == NULL) {
            perror("Error obtener nombre de usuario");
            return;
        }
        if ((group = getgrgid(buf.st_gid)) == NULL) {
            perror("Error obtener nombre del grupo");
            return;
        }
        //Imprimimos todos los datos del fichero
        printf("\t%s\t%ld (%ld)\t%s\t%s %s\t%jd\t%s", date, buf.st_nlink, buf.st_ino, user->pw_name, group->gr_name,
               permisos, buf.st_size, file);
        //Si es -link, incluimos además el fichero al que apunta el link simbólico del fichero
        if (parametros.linkComand) {
            if (readlink(file, directorioLink, buf.st_size + 1) != -1) {
                directorioLink[buf.st_size] = '\0';
                printf(" -> %s\n", directorioLink);
            } else puts("");
        } else puts("");
    } else {
        //Imprimimos la información si no es comando -long
        printf("\t%-8jd\t%s\n", buf.st_size, file);
    }
}

void stats(char *trozos[]) {
    tParametros parametros; //Cargamos los parámetros del comando
    int size = 400, dirIndex;
    char directorio[size];

    if (trozos[1] == NULL) {
        getcwd(directorio, size); //Devuelve el directorio de trabajo actual
        printf("%s", directorio);
    } else {
        dirIndex = leerParametros(trozos, &parametros);  //Guarda direccion del indice del primer archivo
        if(trozos[dirIndex]==NULL) printf("stat: No se ha especificado ningún fichero");
        for (int j = dirIndex; trozos[j] != NULL; j++) {
            statOneFile(trozos[j], parametros); //Llamamos a la función que imprime la información para cada fichero que se pida
        }
    }
}

void removeSubstring(char *string, char *substring) {
    //Esta función busca y elimina de string todas las apariciones de substring. Usada para obtener la dirección relativa de las carpetas que se listan con "list"
    char *match;
    int len = strlen(substring);
    while ((match = strstr(string, substring))) {
        *match = '\0';
        strcat(string, match + len);
    }
}

void readDirFiles(struct dirent *files,struct stat* bufrec,DIR *dir,tParametros parametros){
    //Pasado un parámetro files que conntiene la información de todos los ficheros de un directorio, imprime la información de cada uno de ellos
    while ((files = readdir(dir)) != NULL) {
        if (lstat(files->d_name, bufrec) == -1) {
            perror("****error al acceder");
            return;
        } else if (-strcmp(files->d_name, ".") && -strcmp(files->d_name, "..") || parametros.hidComand) {
            if (parametros.hidComand || files->d_name[0] != '.') {
                printf("\t");
                statOneFile(files->d_name, parametros);
            }
        }
    }
}

void recurseFile(struct dirent *files,struct stat* bufrec,DIR *dir,tParametros parametros,char iniDir[]){
    //Pasado un parámetro files que conntiene la información de todos los ficheros de un directorio, si el fichero que se evalúa es un directorio, se llama recursivamente
    //a listContent para imprimir también la información de los ficheros que estén dentro de ese directorio evaluado
    while ((files = readdir(dir)) != NULL) {
        if (lstat(files->d_name, bufrec) == -1) {
            perror("****error al acceder");
            return;
        }
        if ((bufrec->st_mode & S_IFMT) == S_IFDIR && -strcmp(files->d_name, ".") &&
            -strcmp(files->d_name, "..")) {
            if (parametros.hidComand || files->d_name[0] != '.') {
                listContent(files->d_name, parametros, iniDir);
            }
        }
    }
}

void listContent(char *filename, tParametros parametros, char iniDir[]) {
    struct stat buf, bufrec;
    DIR *dir = NULL;
    struct dirent *files = NULL;
    char ogDir[1000];
    char path[1000];
    if (lstat(filename, &buf) == -1) { //No se han podido volcar los datos del fichero
        perror("****error al acceder");
        return;
    }
    if ((buf.st_mode & S_IFMT) == S_IFDIR) { //Si el fichero es un directorio
        getcwd(ogDir, 1000);
        if(!parametros.recaComand && !parametros.recbComand) printf("************%s\n", filename);
        //Abrimos dicho directorio y nos cambiamos a él
        dir = opendir(filename);
        if (dir == NULL) {
            perror("Error al acceder al directorio");
            return;
        }
        if (chdir(filename) == -1) {
            perror("No se pudo cambiar el directorio");
        }
        else {
            if(parametros.recaComand){
                //Si tenemos parámetro -reca, primero se imprime la información de los directorios
                //antes de acceder recursivamente a sus posibles subdirectorios
                getcwd(path, 1000);
                removeSubstring(path, iniDir);
                printf("************%s\n", path);
                readDirFiles(files,&bufrec,dir,parametros); //Primero se lee
                chdir(ogDir);
                closedir(dir);
                dir = opendir(filename);
                if (dir == NULL) {
                    perror("Error al acceder al directorio");
                    return;
                }

                if (chdir(filename) == -1) perror("No se pudo cambiar el directorio");
                recurseFile(files,&bufrec,dir,parametros,iniDir); //Luego se accede a subdirectorios recursivamente
                chdir(ogDir);
                closedir(dir);
            }
            else if(parametros.recbComand){
                //Si tenemos parámetro -recb, primero se accede recursivamente a todos los subdirectorios que existan,
                //y luego se imprime toda su información desde dentro hacia afuera
                recurseFile(files,&bufrec,dir,parametros,iniDir); //Primero se accede a subdirectorios recursivamente
                getcwd(path, 1000);
                removeSubstring(path, iniDir);
                printf("************%s\n", path);
                chdir(ogDir);
                closedir(dir);
                dir = opendir(filename);
                if (dir == NULL) {
                    perror("Error al acceder al directorio");
                    return;
                }
                if (chdir(filename) == -1) perror("No se pudo cambiar el directorio");
                readDirFiles(files,&bufrec,dir,parametros); //Luego se lee
                chdir(ogDir);
                closedir(dir);
            }
            else{
                //No hay recursividad, se leen los ficheros del directorio sin acceder a subdirectorios y luego se cierra
                readDirFiles(files,&bufrec,dir,parametros);
                chdir(ogDir);
                closedir(dir);
            }
        }
    } else statOneFile(filename, parametros); //El fichero pasado no es un directorio, imprimimos su información
}

void list(char *trozos[]) {
    tParametros parametros;
    int dirIndex;
    char iniDir[1000];
    getcwd(iniDir, 1000);
    if (trozos[1] != NULL) {
        //Leemos todos los parámetros del comando introducido
        dirIndex = leerParametros(trozos, &parametros);
        if (trozos[dirIndex] == NULL) printf("%s", iniDir);
        else {
            for (int j = dirIndex; trozos[j] != NULL; j++) listContent(trozos[j],parametros,iniDir);
        }
    } else stats(trozos); //Sin parámetros, imprime el directorio actual

}

int isDirectory(char* fileName) {    //si es directorio retorna 1, sino 0, si hay un error distinto de no such files retorna -1 y sino 2
    struct stat buf;

    if (lstat(fileName, &buf) == -1) {
        if (strcmp(strerror(errno), "No such file or directory") !=
            0) {    //para que no se pare la ejecucion al no encontrar un fichero o dir
            return 2;
        } else return -1;

    }

    return (buf.st_mode & S_IFMT) == S_IFDIR;
}


int isEmptyDir(char* fileName){ //PreCD: Solo recibe directorios
    int cont=0;
    struct dirent *files=NULL;
    DIR *dir = NULL;
    dir = opendir(fileName);
    if (dir == NULL) {
        perror("Error al acceder al directorio");
        return -1;
    }

    while ((files = readdir(dir)) != NULL) {
        cont++;
    }
    closedir(dir);
    return cont==2;
}

void delete(char *trozos[]) {
    char error[30];
    if (trozos[1] != NULL) {
        //Leemos todos los ficheros que el comando nos pide borrar
        for (int i = 1; trozos[i] != NULL; i++) {
            switch (isDirectory(trozos[i])) { //Evaluamos el resultado de isDirectory para el fichero
                case 0: //Es un fichero
                    if (remove(trozos[i]) == -1) { //Si remove devuelve -1 es que no se pudo borrar el fichero
                        sprintf(error, "Imposible borrar %s", trozos[i]);
                        perror(error);
                        break;
                    }
                    break;
                case 1: //Es un directorio
                    if (rmdir(trozos[i]) == -1) {
                        sprintf(error, "Imposible borrar %s", trozos[i]);
                        perror(error);
                    }
                    else printf("Directorio %s borrado con éxito", trozos[i]);
                    break;
                case -1:
                    //Ha habido algún error para obtener la información del fichero con lstat
                    sprintf(error, "Imposible borrar %s", trozos[i]);
                    perror(error);
                    break;
            }
        }
    } else stats(trozos);

}

void recDelete(char* filename){
    char error[30];
    DIR *dir=NULL;
    struct dirent *file=NULL;
    if (isDirectory(filename) == -1) { //No se pudo obtener la información del fichero filename por alguna razón
        sprintf(error, "Imposible borrar %s", filename);
        perror(error);
    }
    else if(!isDirectory(filename)) remove(filename); //Si se trata de un fichero se borra directamente
    else if(isEmptyDir(filename)) rmdir(filename); //Si se trata de un directorio vacío se borra directamente
    else{
        //Es un directorio lleno, se abre su contenido y accedemos a el
        dir = opendir(filename);
        if (dir == NULL) {
            perror("Error al abrir directorio");
            return;
        }
        chdir(filename);
        while ((file = readdir(dir)) != NULL) {
            //Recorremos todos los ficheros, incluidos posibles subdirectorios, del directorio filename borrándolos todos recursivamente
            if (-strcmp(file->d_name, ".") && -strcmp(file->d_name, "..")) { //Excepto el .. y el . para no acceder recursivamente al directorio padre!
                recDelete(file->d_name);
            }
        }
        //Finalmente cerramos y borramos el directorio original
        chdir("..");
        rmdir(filename);
        closedir(dir);
    }
}

void deltree(char *trozos[]) {
    if (trozos[1] != NULL) {
        //Se intenta borrar recursivamente cada fichero listado en el comando
        for (int i = 1; trozos[i] != NULL; i++) {
            recDelete(trozos[i]);
        }
    } else stats(trozos);
}