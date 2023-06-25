#ifndef ___diretorio_h___
#define ___diretorio_h___

#include <stdio.h>
#include "operacoes.h"
#include "lista.h"
#include "archive.h"
#include "caminho.h"

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador);

void extraiInformacoes (struct lista *lista, FILE *arquivador);

void atualizaLista (size_t tamanho, int ordem, struct lista *lista, int mudaOrdem);

void atualizaMove (struct nol *mover, struct nol *target, struct lista *lista);

void mudaPonteiros (struct nol *target, struct nol *mover, struct lista *lista);

void atualizaListaArchive (FILE *arquivador, struct lista *lista);

void imprimeListaArq (struct lista *lista, FILE *arq);

#endif // ___diretorio_h___