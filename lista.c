#include "lista.h"
#include "util.h"
#include <time.h>
#include <string.h>

struct nol *criaNo (struct stat *dados, char *nome, int pos){
    struct nol *l;
    l = malloc(sizeof (struct nol));
    if (!l)
        return NULL;
    l->perms = dados->st_mode;
    l->tamanho = dados->st_size;
    l->userid = dados->st_uid;
    l->tempo = dados->st_mtime;
    l->nome = nome;
    l->pos = pos;
    l->prox = NULL;
    return l;
}

struct nol *removeElemento (struct lista *lista, char *nome){
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
    lista->tam++;
}

void imprimeLista (struct lista *lista, FILE *arq){
    struct nol *aux;
    char *data;
    aux = lista->inicio;
    while (aux){
        fwrite (aux->nome, sizeof(char), strlen(aux->nome), arq);
        fwrite (&aux->userid, sizeof(int), 1 , arq);
        fwrite (&aux->perms, sizeof(int), 1 , arq);
        fwrite (&aux->tamanho, sizeof(long), 1 , arq);
        data = ctime (&aux->tempo);
        fwrite (data, sizeof(char), strlen(data) , arq);
        fwrite (&aux->pos, sizeof(int), 1 , arq);
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
    return aux;
}

struct lista *destroiLista (struct lista *lista){
    struct nol *aux;
    while (lista->inicio){
        aux = lista->inicio;
        lista->inicio = lista->inicio->prox;
        free (aux);
        aux = NULL;
    }
    lista->tam = 0;
    free (lista);
    return NULL;
}

struct nol *busca (char *nome, struct lista *lista){
    struct nol *aux;
    aux = lista->inicio;
    while (aux){
        if (!strcmp (aux->nome, nome))
            return aux;
        aux = aux->prox;
    }
    return NULL;
}