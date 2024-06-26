#ifndef P0_GENERALFUNCTIONS_H
#define P0_GENERALFUNCTIONS_H

#include "includes.h"

void imprimirPrompt();
void leerEntrada(char *comando);
bool procesarEntrada(tComando comando, tList *L, char *trozos[], tListF *F, tListLM *memL, tListP *procL,char *envp[]);
void procesarComando(char *trozos[], tList *L, tListF *F, tListLM *memL,tListP *procL, char *envp[]);
int TrocearCadena(char *cadena, char *trozos[]);
int stringToInt(const char string[]);
bool tests_run_within_valgrind (void);
void Help(char *trozos[]);

#endif //P0_GENERALFUNCTIONS_H
