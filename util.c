#include "util.h"
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

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador){
    no->perms = st.st_mode;
    no->userid = st.st_uid;
    no->tamanho = st.st_size;
    no->tempo = st.st_mtime;
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

void atualizaLista (size_t tamanho, size_t pos, struct lista *lista){
    struct nol *aux;
    aux = lista->inicio;
    while (aux){
        if (aux->pos > pos)
            aux->pos -= tamanho;
        aux = aux->prox;
    }
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
    printf( (mode & S_IXOTH) ? "x" : "-");
    printf("\t");
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

char *retornaNome (char *path){
    char *aux;
    if (path[0] == '/'){
        aux = malloc (sizeof (char) * (strlen (path) + 2));
        aux[0] = '.';
        aux += 1;
        strncpy (aux, path, strlen (path));
        aux[strlen (path)] = '\0';
        aux -= 1;
        return aux;
    }
    else{
        for (int i = 1; i < strlen (path); i++){
            if (path[i] == '/'){
                aux = malloc (sizeof (char) * (strlen (path) + 3));
                aux[0] = '.';
                aux[1] = '/';
                aux += 2;
                strncpy (aux, path, strlen (path));
                aux[strlen (path)] = '\0';
                aux -= 2;
                return aux;
            }
        }
        aux = malloc (sizeof (char) * (strlen (path) + 1));
        strncpy (aux, path, strlen (path));
        aux[strlen (path)] = '\0';
        return aux;
    }
}

char *retornaNomeArq (char *path){
    int tam = 0, i = strlen (path) - 1;
    while (path[i] != '/'){
        tam++;
        i--;
    }
    char *aux = malloc (sizeof (char) * (tam + 1));
    for (int j = 0; j < tam; j++){
        aux[j] = path[i+1];
        i++;
    }
    aux[tam] = '\0';
    return aux;
}

void extraiDiretorio (struct nol *no, FILE *arquivador){
    char *nomeArq, *aux;
    char path[BUFFER];
    getcwd (path, BUFFER);
    nomeArq = retornaNomeArq (no->nome);
    no->nome += 2;
    aux = strtok (no->nome, "/");
    no->nome -= 2;
    while (strcmp (aux, nomeArq)){
        mkdir (aux, 0777);
        chdir (aux);
        aux = strtok (NULL, "/");
    }
    extraiArquivo (no, arquivador, nomeArq);
    chdir (path);
    free (nomeArq);
}