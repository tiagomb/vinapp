#ifndef ___archive_h___
#define ___archive_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"
#include "caminho.h"
#include "diretorio.h"

void leArquivo (char *arq, FILE *arquivador, struct stat st);

void removeArquivo (struct nol *no, FILE *arquivador, struct lista *lista, size_t offset);

void extraiArquivo (struct nol *no, FILE *arquivador, char *nome);

void imprimePermissoes (mode_t mode);

void refazEspaco (FILE *arquivador, struct nol *aux, struct lista *lista, long int diff);

void abreEspaco (FILE *arquivador, struct nol *mover, struct nol *target, struct lista *lista);

void copiaArquivo (struct nol *target, struct nol *mover, FILE *arquivador);

#endif // ___archive_h___