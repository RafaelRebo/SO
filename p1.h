#ifndef P0_P1_H
#define P0_P1_H

#include "includes.h"



void inicializarParametros(tParametros *parametros);
int leerParametros(char *trozos[], tParametros *parametros);
void create(char *trozos[]);
void statOneFile(char *file, tParametros parametros);
void stats(char *trozos[]);
void removeSubstring(char *string, char *substring);
void listContent(char *filename, tParametros parametros, char iniDir[]);
void list(char *trozos[]);
int isDirectory(char* fileName);
int isEmptyDir(char* fileName);
void delete(char *trozos[]);
void recDelete(char* filename);
void deltree(char *trozos[]);

#endif //P0_P1_H
