#include "diretorio.h"
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador){
    no->perms = st.st_mode;
    no->userid = st.st_uid;
    no->tamanho = st.st_size;
    no->tempo = st.st_mtime;
}

void extraiInformacoes (struct lista *lista, FILE *arquivador){
    size_t pos;
    int tam_lista, tam_arq;
    struct nol *aux;
    fread (&pos, sizeof(size_t), 1, arquivador);
    fseek (arquivador, pos, SEEK_SET);
    fread (&tam_lista, sizeof(int), 1, arquivador);
    for (int i = 0; i < tam_lista; i++){
        aux = malloc (sizeof (struct nol));
        aux->prox = NULL;
        aux->ant = NULL;
        fread (&tam_arq, sizeof(int), 1, arquivador);
        aux->nome = malloc ((tam_arq+1)*sizeof(char));
        fread (aux->nome, sizeof(char), tam_arq, arquivador);
        aux->nome[tam_arq] = '\0';
        fread (&aux->userid, sizeof(uid_t), 1, arquivador);
        fread (&aux->perms, sizeof(mode_t), 1, arquivador);
        fread (&aux->tamanho, sizeof(off_t), 1, arquivador);
        fread (&aux->tempo, sizeof(time_t), 1, arquivador);
        fread (&aux->pos, sizeof(size_t), 1, arquivador);
        adicionaNo (lista, aux);
    }
}

void atualizaLista (size_t tamanho, size_t pos, struct lista *lista){
    struct nol *aux;
    aux = lista->inicio;
    while (aux){
        if (aux->pos > pos)
            aux->pos -= tamanho;
        aux = aux->prox;
    }
}

void atualizaMove (struct nol *mover, struct nol *target, struct lista *lista){
    struct nol *aux;
    aux = lista->inicio;
    if (mover->pos > target->pos){
        while (aux){
            if (aux->pos > target->pos && aux->pos < mover->pos)
                aux->pos += mover->tamanho;
            aux = aux->prox;
        }
    }
    else{
        while (aux){
            if (aux->pos > mover->pos && aux->pos <= target->pos)
                aux->pos -= mover->tamanho;
            aux = aux->prox;
        }
    }
}

void mudaPonteiros (struct nol *target, struct nol *mover, struct lista *lista){
    if (lista->inicio == mover)
        lista->inicio = mover->prox;
    if (lista->fim == mover)
        lista->fim = mover->ant;
    if (mover->prox)
        mover->prox->ant = mover->ant;
    if (mover->ant)
        mover->ant->prox = mover->prox;
    if (target->prox)
        target->prox->ant = mover;
    mover->prox = target->prox;
    mover->ant = target;
    target->prox = mover;
}

void atualizaListaArchive (FILE *arquivador, struct lista *lista){
    size_t pos;
    fseek (arquivador, 0, SEEK_END);
    pos = ftell (arquivador);
    rewind (arquivador);
    fwrite (&pos, sizeof(size_t), 1, arquivador);
    fseek (arquivador, 0, SEEK_END);
    imprimeListaArq (lista, arquivador);
    fclose (arquivador);
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
        aux = aux->prox;
    }
}