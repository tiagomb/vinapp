#ifndef ___util_h___
#define ___util_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st);

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador);

void removeArquivo (struct nol *no, FILE *arquivador);

void extraiInformacoes (struct lista *lista, FILE *arquivador);

void extraiArquivo (struct nol *no, FILE *arquivador);

void atualizaLista (int tamanho, int pos, struct lista *lista);

void imprimePermissoes (mode_t mode);

void imprimeOpcoes ();

#endif // ___util_h___