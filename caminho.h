#ifndef ___caminho_h___
#define ___caminho_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"
#include "diretorio.h"
#include "archive.h"

char *retornaNome (char *path);

char *retornaNomeArq (char *path);

void extraiDiretorio (struct nol *no, FILE *arquivador);

#endif // ___caminho_h___