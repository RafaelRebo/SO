#include "includes.h"

char* getUserFromUID(uid_t uid){
    struct passwd *user;
    if ((user = getpwuid(uid)) == NULL) {
        perror("stat: Error al obtener nombre de usuario");
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