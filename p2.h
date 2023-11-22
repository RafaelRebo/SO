#ifndef P0_P2_H
#define P0_P2_H

#include "includes.h"
#include "listamemoria.h"

void Cmd_malloc(char* trozos[],tListLM* memL);
void Cmd_shared(char *trozos[],tListLM* M);
void * MapearFichero (char * fichero, int protection, tListLM* memL);
void CmdMmap(char *arg[], tListLM* memL);
void Cmd_mmap(char* trozos[], tListLM* memL);
void CmdRead (char *ar[]);
void Cmd_read (char* trozos[]);
void Cmd_memdump (char* trozos[]);
void Cmd_memfill (char* trozos[]);
void Cmd_write(char* trozos[]);
void Cmd_mem(char* trozos[],tListLM M);
void Cmd_recurse(char* trozos[]);


#endif //P0_P2_H
