#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

struct nol{
    char *nome;
    int pos;
    struct stat *info;
    struct nol *prox;
};

struct lista{
    int tam;
    struct nol *inicio;
    struct nol *fim;
};

struct nol *criaNo (struct stat *dados, char *nome, int pos);

struct nol *removeElemento (struct lista *lista, char *nome);

void adicionaNaCauda (struct lista *lista, struct stat *dados, char *nome, int pos);

void imprimeLista (struct lista *lista);

int estaVazia (struct lista *lista);

struct lista *inicializaLista ();

struct lista *destroiLista (struct lista *lista);
