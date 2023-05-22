#include "lista.h"

struct nol *criaNo (struct stat *dados, char *nome, int pos){
    struct nol *l;
    l = malloc(sizeof (struct nol));
    if (!l)
        return NULL;
    l->info = dados;
    l->nome = nome;
    l->pos = pos;
    l->prox = NULL;
    return l;
}

struct nol *removeElemento (struct lista *lista, char *nome){
    int dado, pos;
    struct nol *aux;
    aux = lista->inicio;
    if (aux->nome == nome){
        lista->inicio = lista->inicio->prox;
        return aux;
    }
    while (aux->prox && aux->prox->nome != nome)
        aux = aux->prox;
    if (aux->prox){
        aux->prox = aux->prox->prox;
        return aux->prox;
    }
    return NULL;
}

void adicionaNaCauda (struct lista *lista, struct stat *dados, char *nome, int pos){
    struct nol* novo;
    novo = criaNo(dados, nome, pos);
    if (!novo){
        fprintf (stderr, "Falha de alocação");
        exit (1);
    }
    else if (estaVazia (lista)){
        lista->inicio = novo;
        lista->fim = novo;
    }
    else{
        lista->fim->prox = novo;
        lista->fim = novo;
    }
}

void imprimeLista (struct lista *lista){
    struct nol *aux;
    int i = 1;
    aux = lista->inicio;
    while (aux){
        aux = aux->prox;
    }
}

int estaVazia (struct lista *lista){
    if (!lista->tam)
        return 1;
    return 0;
}

struct lista *inicializaLista (){
    struct lista *aux;
    aux = malloc (sizeof (struct lista));
    aux->tam = 0;
    aux->inicio = NULL;
    aux->fim = NULL;
}

struct lista *destroiLista (struct lista *lista){
    struct nol *aux;
    while (lista->inicio){
        aux = lista->inicio;
        lista->inicio = lista->inicio->prox;
        free (aux);
        aux = NULL;
    }
    free (lista);
    return NULL;
}