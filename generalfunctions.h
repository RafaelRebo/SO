#ifndef P0_GENERALFUNCTIONS_H
#define P0_GENERALFUNCTIONS_H

#include "includes.h"

void imprimirPrompt();
void leerEntrada(char *comando);
bool procesarEntrada(tComando comando, tList *L, char *trozos[], tListF *F, tListLM *memL);
void procesarComando(char *trozos[], tList *L, tListF *F, tListLM *memL);
int TrocearCadena(char *cadena, char *trozos[]);
void Help(char *trozos[]);

#endif //P0_GENERALFUNCTIONS_H
