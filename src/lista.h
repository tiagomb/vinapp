#ifndef ___lista_h___
#define ___lista_h___

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

struct nol{
    char *nome;
    int ordem;
    size_t pos;
    mode_t perms;
    uid_t userid;
    gid_t groupid;
    off_t tamanho;
    time_t tempo;
    struct nol *prox;
    struct nol *ant;
};

struct lista{
    int tam;
    struct nol *inicio;
    struct nol *fim;
};

struct nol *criaNo (struct stat *dados, char *nome, size_t pos, int ordem);

struct nol *removeElemento (struct lista *lista, char *nome);

void adicionaNo (struct lista *lista, struct nol *no);

void adicionaNaCauda (struct lista *lista, struct stat *dados, char *nome, size_t pos, int ordem);

int estaVazia (struct lista *lista);

struct lista *inicializaLista ();

struct lista *destroiLista (struct lista *lista);

struct nol *busca (char *nome, struct lista *lista);

#endif // ___lista_h___
