#ifndef ___util_h___
#define ___util_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st);

void removeArquivo (struct nol *no, FILE *arquivador);

#endif // ___util_h___