#include "lista.h"
#include "util.h"
#include <time.h>
#include <string.h>

struct nol *criaNo (struct stat *dados, char *nome, size_t pos, int ordem){
    struct nol *l;
    l = malloc(sizeof (struct nol));
    if (!l)
        return NULL;
    if (dados){
        l->perms = dados->st_mode;
        l->tamanho = dados->st_size;
        l->userid = dados->st_uid;
        l->tempo = dados->st_mtime;
    }
    l->nome = malloc ((strlen(nome)+1)*sizeof(char));
    strncpy (l->nome, nome, strlen(nome));
    l->nome[strlen(nome)] = '\0';
    l->pos = pos;
    l->ordem = ordem;
    l->prox = NULL;
    l->ant = NULL;
    return l;
}

struct nol *removeElemento (struct lista *lista, char *nome){
    struct nol *aux;
    aux = lista->inicio;
    if (!strcmp(aux->nome, nome)){
        lista->inicio = lista->inicio->prox;
        lista->tam--;
        return aux;
    }
    while (aux->prox && strcmp(aux->prox->nome, nome))
        aux = aux->prox;
    if (aux->prox){
        aux->prox = aux->prox->prox;
        aux->prox->ant = aux;
        lista->tam--;
        return aux->prox;
    }
    return NULL;
}

void adicionaNo (struct lista *lista, struct nol *no){
    if (estaVazia (lista)){
        lista->inicio = no;
        lista->fim = no;
    }
    else{
        lista->fim->prox = no;
        no->ant = lista->fim;
        lista->fim = no;
    }
    lista->tam++;
}

void adicionaNaCauda (struct lista *lista, struct stat *dados, char *nome, size_t pos, int ordem){
    struct nol* novo;
    novo = criaNo(dados, nome, pos, ordem);
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
        novo->ant = lista->fim;
        lista->fim = novo;
    }
    lista->tam++;
}

void imprimeListaArq (struct lista *lista, FILE *arq){
    struct nol *aux;
    int tam;
    if (estaVazia (lista))
        return;
    aux = lista->inicio;
    fwrite (&lista->tam, sizeof(int), 1 , arq);
    while (aux){
        tam = strlen(aux->nome);
        fwrite (&tam, sizeof(int), 1 , arq);
        fwrite (aux->nome, sizeof(char), strlen(aux->nome), arq);
        fwrite (&aux->userid, sizeof(uid_t), 1 , arq);
        fwrite (&aux->perms, sizeof(mode_t), 1 , arq);
        fwrite (&aux->tamanho, sizeof(off_t), 1 , arq);
        fwrite (&aux->tempo, sizeof(time_t), 1 , arq);
        fwrite (&aux->pos, sizeof(size_t), 1 , arq);
        fwrite (&aux->ordem, sizeof(int), 1 , arq);
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
        free (aux->nome);
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