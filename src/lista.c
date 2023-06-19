#include "lista.h"
#include "archive.h"
#include <time.h>
#include <string.h>

/*A função abaixo cria um nó com as informações passadas como paramêtro e retorna um ponteiro para esse nó.
Caso a alocação falhe, retorna NULL*/
struct nol *criaNo (struct stat *dados, char *nome, size_t pos){
    struct nol *l;
    l = malloc(sizeof (struct nol));
    if (!l)
        return NULL;
    l->perms = dados->st_mode;
    l->tamanho = dados->st_size;
    l->userid = dados->st_uid;
    l->tempo = dados->st_mtime;
    l->nome = malloc ((strlen(nome)+1)*sizeof(char));
    strncpy (l->nome, nome, strlen(nome));
    l->nome[strlen(nome)] = '\0';
    l->pos = pos;
    l->prox = NULL;
    l->ant = NULL;
    return l;
}

/*A seguinte função remove um nó da lista, alterando os ponteiros dos nós anterior e próximo ao elemento removido para que 
passem a apontar um para o outro. A função retorna o nó removido, cabendo a quem chamou a função liberar a memória.*/
struct nol *removeElemento (struct lista *lista, char *nome){
    struct nol *aux;
    aux = lista->inicio;
    if (!strcmp(aux->nome, nome)){
        lista->inicio = lista->inicio->prox;
        lista->tam--;
        return aux;
    }
    while (aux && strcmp(aux->nome, nome))
        aux = aux->prox;
    if (aux){
        aux->ant->prox = aux->prox;
        if (aux->prox)
            aux->prox->ant = aux->ant;
        aux->ant = NULL;
        aux->prox = NULL;
        lista->tam--;
        return aux;
    }
    return NULL;
}

/*A função recebe um nó pronto e adiciona na cauda*/
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

/*A função abaixo cria um nó com as informações passadas como paramêtro, e insere-o na cauda*/
void adicionaNaCauda (struct lista *lista, struct stat *dados, char *nome, size_t pos){
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
        novo->ant = lista->fim;
        lista->fim = novo;
    }
    lista->tam++;
}

/*Verifica se a lista está vazia. Caso esteja, retorna 1, senão, retorna 0*/
int estaVazia (struct lista *lista){
    if (!lista->tam)
        return 1;
    return 0;
}

/*Aloca espaço para uma struct lista, inicializando os campos em 0 e NULL, e retorna um ponteiro para o espaço alocado.
Caso haja falha, imprime uma mensagem de erro e sai com código de erro 1.*/
struct lista *inicializaLista (){
    struct lista *aux;
    aux = malloc (sizeof (struct lista));
    if (!aux){
        fprintf (stderr, "Falha de alocação na estrutura lista.\n");
        exit(1);
    }
    aux->tam = 0;
    aux->inicio = NULL;
    aux->fim = NULL;
    return aux;
}

/*A função abaixo é responsável por liberar toda a memória alocada para a lista, alterando os ponteiros para NULL após a liberação*/
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

/*A função abaixo busca um nó pelo nome, e retorna um ponteiro para esse nó caso seja encontrado. Caso contrário, retorna NULL*/
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
