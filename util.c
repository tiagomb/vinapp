#include "util.h"
#include <unistd.h>

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st){
    FILE *arquivo;
    int leituras;
    if (!(arquivo = fopen (arq, "r"))){
        fprintf (stderr, "Falha ao abrir %s\n", arq);
        exit (1);
    }
    leituras = st.st_size/BUFFER;
    for (int j = 0; j < leituras; j++){
        fread(buffer, sizeof(char), BUFFER, arquivo);
        fwrite(buffer, sizeof(char), BUFFER, arquivador);
    }
    if (st.st_size%BUFFER){
        fread(buffer, sizeof(char), st.st_size%BUFFER, arquivo);
        fwrite(buffer, sizeof(char), st.st_size%BUFFER, arquivador);
    }
    fclose (arquivo);
}

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador){
    no->perms = st.st_mode;
    no->userid = st.st_uid;
    no->tamanho = st.st_size;
    no->tempo = st.st_mtime;
    fseek (arquivador, 0, SEEK_END);
    no->pos = ftell (arquivador);
}

void removeArquivo (struct nol *no, FILE *arquivador){
    char buffer[BUFFER];
    int leituras, tam, pos, aux;
    if (!no->prox)
        ftruncate (fileno(arquivador), no->pos);
    else{
        rewind (arquivador);
        fread (&pos, sizeof(int), 1, arquivador);
        fseek (arquivador, 0, SEEK_END);
        aux = ftell(arquivador);
        fseek (arquivador, -no->prox->pos, SEEK_END);
        tam = ftell (arquivador) - (aux - pos);
        leituras = tam/BUFFER;
        for (int i = 0; i < leituras; i++){
            fseek (arquivador, no->prox->pos + i*BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), BUFFER, arquivador);
            fseek (arquivador, no->pos + i*BUFFER, SEEK_SET);
            fwrite (buffer, sizeof(char), BUFFER, arquivador);
        }
        if (tam%BUFFER){
            fseek (arquivador, no->prox->pos + leituras*BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), tam%BUFFER, arquivador);
            fseek (arquivador, no->pos + leituras*BUFFER, SEEK_SET);
            fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
        }
        rewind (arquivador);
        ftruncate (fileno(arquivador), no->pos + tam);
        tam = no->tamanho;
        while (no->prox){
            no->prox->pos -= tam;
            no = no->prox;
        }
    }
}

void extraiInformacoes (struct lista *lista, FILE *arquivador){
    int pos, tam_arq, tam_lista;
    struct nol *aux;
    fread (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, pos, SEEK_SET);
    fread (&tam_lista, sizeof(int), 1, arquivador);
    for (int i = 0; i < tam_lista; i++){
        aux = malloc (sizeof (struct nol));
        aux->prox = NULL;
        fread (&tam_arq, sizeof(int), 1, arquivador);
        aux->nome = malloc ((tam_arq+1)*sizeof(char));
        fread (aux->nome, sizeof(char), tam_arq, arquivador);
        aux->nome[tam_arq] = '\0';
        fread (&aux->userid, sizeof(int), 1, arquivador);
        fread (&aux->perms, sizeof(int), 1, arquivador);
        fread (&aux->tamanho, sizeof(long), 1, arquivador);
        fread (&aux->tempo, sizeof(int), 1, arquivador);
        fread (&aux->pos, sizeof(int), 1, arquivador);
        adicionaNo (lista, aux);
    }
}