#include "archive.h"
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>

void leArquivo (char *arq, char *buffer, FILE *arquivador, struct stat st){
    FILE *arquivo;
    size_t leituras;
    if (!(arquivo = fopen (arq, "r"))){
        fprintf (stderr, "Falha ao abrir %s\n", arq);
        exit (1);
    }
    leituras = st.st_size/BUFFER;
    for (size_t j = 0; j < leituras; j++){
        fread(buffer, sizeof(char), BUFFER, arquivo);
        fwrite(buffer, sizeof(char), BUFFER, arquivador);
    }
    if (st.st_size%BUFFER){
        fread(buffer, sizeof(char), st.st_size%BUFFER, arquivo);
        fwrite(buffer, sizeof(char), st.st_size%BUFFER, arquivador);
    }
    fclose (arquivo);
}

void removeArquivo (struct nol *no, FILE *arquivador, struct lista *lista, size_t offset){
    char buffer[BUFFER];
    size_t leituras, tam, fimArqs, fim;
    fimArqs = lista->fim->pos + lista->fim->tamanho + offset;
    fseek (arquivador, fimArqs, SEEK_SET);
    fim = ftell (arquivador);
    tam = fim - (no->pos + no->tamanho);
    leituras = tam/BUFFER;
    for (size_t i = 0; i < leituras; i++){
        fseek (arquivador, no->pos + no->tamanho + i*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, no->pos + i*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), BUFFER, arquivador);
    }
    if (tam%BUFFER){
        fseek (arquivador, no->pos + no->tamanho + leituras*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), tam%BUFFER, arquivador);
        fseek (arquivador, no->pos + leituras*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
    }
    rewind (arquivador);
    ftruncate (fileno(arquivador), no->pos + tam);
    fseek (arquivador, 0, SEEK_END);
}

void extraiArquivo (struct nol *no, FILE *arquivador, char *nome){
    char buffer[BUFFER];
    size_t leituras;
    FILE *arquivo;
    arquivo = fopen (nome, "w");
    fseek (arquivador, no->pos, SEEK_SET);
    leituras = no->tamanho/BUFFER;
    for (size_t i = 0; i < leituras; i++){
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fwrite (buffer, sizeof(char), BUFFER, arquivo);
    }
    if (no->tamanho%BUFFER){
        fread (buffer, sizeof(char), no->tamanho%BUFFER, arquivador);
        fwrite (buffer, sizeof(char), no->tamanho%BUFFER, arquivo);
    }
    fclose (arquivo);
}

void imprimePermissoes (mode_t mode){
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x " : "- ");
}

void refazEspaco (FILE *arquivador, struct nol *aux, struct lista *lista, long int diff){
    size_t tam, leituras, fim;
    char buffer[BUFFER];
    if (diff == 0)
        return;
    size_t fimArqs = lista->fim->pos + lista->fim->tamanho;
    fseek (arquivador, fimArqs, SEEK_SET);
    fim = ftell (arquivador);
    tam = fim - (aux->pos + aux->tamanho);
    leituras = tam/BUFFER;
    if (diff > 0){
        for (size_t i = 0; i < leituras; i++){
            fseek (arquivador, fimArqs - (i+1) * BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), BUFFER, arquivador);
            fseek (arquivador, fimArqs - (i+1) * BUFFER + diff , SEEK_SET);
            fwrite (buffer, sizeof(char), BUFFER, arquivador);
        }
        if (tam%BUFFER){
            fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER), SEEK_SET);
            fread (buffer, sizeof(char), tam%BUFFER, arquivador);
            fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER) + diff, SEEK_SET);
            fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
        }
    }
    else{
        for (size_t i = 0; i < leituras; i++){
            fseek (arquivador, aux->pos + aux->tamanho + i*BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), BUFFER, arquivador);
            fseek (arquivador, aux->pos + aux->tamanho +i*BUFFER + diff , SEEK_SET);
            fwrite (buffer, sizeof(char), BUFFER, arquivador);
        }
        if (tam%BUFFER){
            fseek (arquivador, aux->pos + aux->tamanho + leituras*BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), tam%BUFFER, arquivador);
            fseek (arquivador, aux->pos + aux->tamanho + leituras*BUFFER + diff, SEEK_SET);
            fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
        }
    }
    ftruncate (fileno(arquivador), fimArqs + diff);
}

void abreEspaco (FILE *arquivador, struct nol *mover, struct nol *target, struct lista *lista){
    size_t tam, leituras, fim;
    char buffer[BUFFER];
    size_t fimArqs = lista->fim->pos + lista->fim->tamanho;
    fseek (arquivador, fimArqs, SEEK_SET);
    fim = ftell (arquivador);
    tam = fim - (target->pos + target->tamanho);
    leituras = tam/BUFFER;
    for (size_t i = 0; i < leituras; i++){
        fseek (arquivador, fimArqs - (i+1) * BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, fimArqs - (i+1) * BUFFER + mover->tamanho , SEEK_SET);
        fwrite (buffer, sizeof(char), BUFFER, arquivador);
    }
    if (tam%BUFFER){
        fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER), SEEK_SET);
        fread (buffer, sizeof(char), tam%BUFFER, arquivador);
        fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER) + mover->tamanho, SEEK_SET);
        fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
    }
    ftruncate (fileno(arquivador), fimArqs + mover->tamanho);
}

void copiaArquivo (struct nol *target, struct nol *mover, FILE *arquivador){
    size_t leituras;
    char buffer[BUFFER];
    leituras = mover->tamanho/BUFFER;
    for (size_t i = 0; i < leituras; i++){
        fseek (arquivador, mover->pos + i*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, target->pos + target->tamanho + i*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), BUFFER, arquivador);
    }
    if (mover->tamanho%BUFFER){
        fseek (arquivador, mover->pos + leituras*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), mover->tamanho%BUFFER, arquivador);
        fseek (arquivador, target->pos + target->tamanho + leituras*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), mover->tamanho%BUFFER, arquivador);
    }
}