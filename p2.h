#ifndef P0_P2_H
#define P0_P2_H

#include "includes.h"
#include "listamemoria.h"

void Cmd_malloc(char* trozos[],tListLM* memL);
void * MapearFichero (char * fichero, int protection, tListLM* memL);
void CmdMmap(char *arg[], tListLM* memL);
void Cmd_mmap(char* trozos[], tListLM* memL);


#endif //P0_P2_H
