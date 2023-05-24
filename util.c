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
    no->pos = fseek (arquivador, 0, SEEK_END);
}

void removeArquivo (struct nol *no, FILE *arquivador){
    char buffer[BUFFER];
    int leituras, tam;
    tam = fseek (arquivador, 0, SEEK_END) - no->pos - no->tamanho;
    fseek (arquivador, no->pos + no->tamanho, SEEK_SET);
    leituras = tam/BUFFER;
    for (int i = 0; i < leituras; i++){
        fread(buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, -(no->tamanho + BUFFER), SEEK_CUR);
        fwrite(buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, no->tamanho + BUFFER, SEEK_CUR);
    }
    if (tam%BUFFER){
        fread(buffer, sizeof(char), tam%BUFFER, arquivador);
        fseek (arquivador, -(no->tamanho + tam%BUFFER), SEEK_CUR);
        fwrite(buffer, sizeof(char), tam%BUFFER, arquivador);
        fseek (arquivador, no->tamanho + tam%BUFFER, SEEK_CUR);
    }
    rewind (arquivador);
    ftruncate (fileno(arquivador), no->pos + tam);
    tam = no->tamanho;
    while (no->prox){
        no->prox->pos -= tam;
        no = no->prox;
    }
}

void extraiInformacoes (struct lista *lista, FILE *arquivador){
    int pos, tam;
    struct nol *aux;
    fseek (arquivador, 0, SEEK_END);
    if ((ftell(arquivador) == 0))
        return;
    rewind (arquivador);
    fread (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, pos, SEEK_SET);
    while (!feof(arquivador)){
        aux = adicionaNaCauda (lista, NULL, NULL, 0);
        fread (&tam, sizeof(int), 1, arquivador);
        aux->nome = malloc (tam*sizeof(char));
        fread (aux->nome, sizeof(char), tam, arquivador);
        fread (&aux->userid, sizeof(int), 1, arquivador);
        fread (&aux->perms, sizeof(int), 1, arquivador);
        fread (&aux->tamanho, sizeof(long), 1, arquivador);
        fread (&aux->tempo, sizeof(int), 1, arquivador);
        fread (&aux->pos, sizeof(int), 1, arquivador);
        printf ("%s\n", aux->nome);
    }
}