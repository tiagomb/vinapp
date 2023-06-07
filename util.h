#ifndef ___util_h___
#define ___util_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st);

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador);

void removeArquivo (struct nol *no, FILE *arquivador, struct lista *lista, long int offset);

void extraiInformacoes (struct lista *lista, FILE *arquivador);

void extraiArquivo (struct nol *no, FILE *arquivador);

void atualizaLista (int tamanho, int pos, struct lista *lista);

void imprimePermissoes (mode_t mode);

void refazEspaco (FILE *arquivador, struct nol *aux, struct lista *lista, long int diff);

void abreEspaco (FILE *arquivador, struct nol *mover, struct nol *target, struct lista *lista);

void copiaArquivo (struct nol *target, struct nol *mover, FILE *arquivador);

void atualizaMove (struct nol *mover, struct nol *target, struct lista *lista);

void mudaPonteiros (struct nol *target, struct nol *mover, struct lista *lista);

#endif // ___util_h___