#ifndef ___operacoes_h___
#define ___operacoes_h___

#include <stdio.h>
#include "lista.h"

#define BUFFER 1024

void inclui (struct lista *lista, char **args, char modo);

void extrai (struct lista *lista, char **args);

void exclui (struct lista *lista, char **args);

void move (struct lista *lista, char *target, char **args);

void imprimeInformacoes (struct lista *lista, char **args);

void imprimeOpcoes ();

#endif // ___operacoes_h___