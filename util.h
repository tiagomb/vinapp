#ifndef ___util_h___
#define ___util_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st);

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador, int ordem);

void removeArquivo (struct nol *no, FILE *arquivador);

void extraiInformacoes (struct lista *lista, FILE *arquivador);

void extraiArquivo (struct nol *no, FILE *arquivador);

void atualizaLista (int tamanho, int pos, struct lista *lista);

void imprimePermissoes (mode_t mode);

void abreEspaco (FILE *arquivador, struct nol *aux, struct nol *aux1);

void moveArquivo (FILE *arquivador, struct nol *aux, struct nol *aux1, int pos);

void preencheVazio (FILE *arquivador, struct nol *aux, struct nol *aux1, int pos);

void atualizaOrdem (struct lista *lista, struct nol *aux, struct nol *aux1);

void mudaPonteiros (struct nol *aux, struct nol *aux1);

#endif // ___util_h___