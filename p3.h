#ifndef P0_P3_H
#define P0_P3_H

#define MAXVAR 1000
#include "listaprocesos.h"

void Cmd_uid(char* trozos[]);
void Cmd_showvar(char* trozos[], char *envp[]);
void Cmd_changevar(char* trozos[], char *envp[]);
void Cmd_fork (char *tr[], tListP* Lproc);
void exec (char* trozos[], char *envp[]);
void jobs (char* trozos[], tListP Lproc);
void job (char* trozos[], tListP Lproc);
void runProcess(char* trozos[], tListP* Lproc);

#endif //P0_P3_H
