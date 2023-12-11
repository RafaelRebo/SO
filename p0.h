#ifndef P0_P0_H
#define P0_P0_H

#include "includes.h"
#include "lista.h"
#include "listaficheros.h"
#include "listaprocesos.h"


void authors(char *trozos[]);
void pid(char *trozos[]);
void chgdir(char *trozos[]);
void datetime(char *trozos[]);
void hist(char *trozos[], tList *L);
void command(char *trozos[], tList *L, tListF *F, tListLM *memL, tListP *procL, char *envp[]);
void Cmd_open(char *trozos[], tListF *F);
void Cmd_close(char *trozos[], tListF *F);
void Cmd_dup(char *trozos[], tListF *F);
void infosys(char *trozos[]);

#endif //P0_P0_H
