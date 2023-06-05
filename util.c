#include "util.h"
#include <unistd.h>
#include <pwd.h>

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

void atualizaNo (struct nol *no, struct stat st, FILE *arquivador, int ordem){
    no->perms = st.st_mode;
    no->userid = st.st_uid;
    no->tamanho = st.st_size;
    no->tempo = st.st_mtime;
    no->pos = ftell (arquivador);
    no->ordem = ordem;
}

void removeArquivo (struct nol *no, FILE *arquivador){
    char buffer[BUFFER];
    int leituras, tam, pos, aux;
    rewind (arquivador);
    fread (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, 0, SEEK_END);
    aux = ftell(arquivador);
    fseek (arquivador, -(no->pos + no->tamanho), SEEK_END);
    tam = ftell (arquivador) - (aux - pos);
    leituras = tam/BUFFER;
    for (int i = 0; i < leituras; i++){
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
        fread (&aux->userid, sizeof(uid_t), 1, arquivador);
        fread (&aux->perms, sizeof(mode_t), 1, arquivador);
        fread (&aux->tamanho, sizeof(off_t), 1, arquivador);
        fread (&aux->tempo, sizeof(time_t), 1, arquivador);
        fread (&aux->pos, sizeof(size_t), 1, arquivador);
        fread (&aux->ordem, sizeof(int), 1, arquivador);
        adicionaNo (lista, aux);
    }
}

void extraiArquivo (struct nol *no, FILE *arquivador){
    char buffer[BUFFER];
    int leituras;
    FILE *arquivo;
    arquivo = fopen (no->nome, "w");
    fseek (arquivador, no->pos, SEEK_SET);
    leituras = no->tamanho/BUFFER;
    for (int i = 0; i < leituras; i++){
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fwrite (buffer, sizeof(char), BUFFER, arquivo);
    }
    if (no->tamanho%BUFFER){
        fread (buffer, sizeof(char), no->tamanho%BUFFER, arquivador);
        fwrite (buffer, sizeof(char), no->tamanho%BUFFER, arquivo);
    }
    fclose (arquivo);
}

void atualizaLista (int tamanho, int pos, struct lista *lista){
    struct nol *aux;
    aux = lista->inicio;
    while (aux){
        if (aux->pos > pos){
            aux->pos -= tamanho;
            aux->ordem--;
        }
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

void refazEspaco (FILE *arquivador, struct nol *aux, struct lista *lista, size_t diff){
    int tam, leituras, fim;
    char buffer[BUFFER];
    if (diff == 0)
        return;
    if (diff > 0){
        int fimArqs = lista->fim->pos + lista->fim->tamanho;
        fseek (arquivador, fimArqs, SEEK_SET);
        fim = ftell (arquivador);
        tam = fim - (aux->pos + aux->tamanho);
        leituras = tam/BUFFER;
        for (int i = 0; i < leituras; i++){
            fseek (arquivador, fimArqs - (i+1) * BUFFER, SEEK_SET);
            fread (buffer, sizeof(char), BUFFER, arquivador);
            fseek (arquivador, fimArqs - (i+1) * BUFFER + diff , SEEK_SET);
            fwrite (buffer, sizeof(char), BUFFER, arquivador);
        }
        if (tam%BUFFER){
            fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER), SEEK_SET);
            fread (buffer, sizeof(char), BUFFER, arquivador);
            fseek (arquivador, fimArqs - (leituras * BUFFER + tam%BUFFER) + diff, SEEK_SET);
            fwrite (buffer, sizeof(char), BUFFER, arquivador);
        }
    }
    else{
        int fimArqs = lista->fim->pos + lista->fim->tamanho;
        fseek (arquivador, fimArqs, SEEK_SET);
        fim = ftell (arquivador);
        tam = fim - (aux->pos + aux->tamanho);
        leituras = tam/BUFFER;
        for (int i = 0; i < leituras; i++){
            fseek (arquivador, aux->pos + aux->tamanho , SEEK_SET);
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
}

/*void moveArquivo (FILE *arquivador, struct nol *aux, struct nol *aux1, int pos){
    int leituras;
    char buffer[BUFFER];
    leituras = aux1->tamanho / BUFFER;
    for (int i = 0; i < leituras; i++){
        fseek (arquivador, pos + i*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, aux->pos + i*BUFFER + aux->tamanho, SEEK_SET);
        fwrite (buffer, sizeof(char), BUFFER, arquivador);
    }
    if (aux1->tamanho % BUFFER){
        fseek (arquivador, pos + leituras*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), aux1->tamanho % BUFFER, arquivador);
        fseek (arquivador, aux->pos + leituras*BUFFER + aux->tamanho, SEEK_SET);
        fwrite (buffer, sizeof(char), aux1->tamanho % BUFFER, arquivador);
    }
    fseek (arquivador, 0, SEEK_END);
}

void preencheVazio (FILE *arquivador, struct nol *aux, struct nol *aux1, int pos){
    int leituras, tam;
    char buffer[BUFFER];
    tam = ftell (arquivador);
    leituras = tam / BUFFER;
    for (int i = 0; i < leituras; i++){
        fseek (arquivador, pos + aux1->tamanho + i*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), BUFFER, arquivador);
        fseek (arquivador, pos + i*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), BUFFER, arquivador);
    }
    if (tam%BUFFER){
        fseek (arquivador, pos + aux1->tamanho + leituras*BUFFER, SEEK_SET);
        fread (buffer, sizeof(char), tam%BUFFER, arquivador);
        fseek (arquivador, pos + leituras*BUFFER, SEEK_SET);
        fwrite (buffer, sizeof(char), tam%BUFFER, arquivador);
    }
    rewind (arquivador);
    ftruncate (fileno(arquivador), pos + tam);
    fseek (arquivador, 0, SEEK_END);
}

void atualizaOrdem (struct lista *lista, struct nol *aux, struct nol *aux1){
    struct nol *aux2;
    aux2 = lista->inicio;
    if (aux1->ordem > aux->ordem){
        while (aux2){
            if (aux2->ordem > aux->ordem && aux2->ordem < aux1->ordem){
                aux2->ordem++;
                aux2->pos += aux1->tamanho;
            }
            aux2 = aux2->prox;
        }
        aux1->ordem = aux->ordem + 1;
    }
    else{
        while (aux2){
            if (aux2->ordem < aux->ordem && aux2->ordem > aux1->ordem){
                aux2->ordem--;
                aux2->pos -= aux1->tamanho;
            }
            aux2 = aux2->prox;
        }
        aux1->ordem = aux->ordem - 1;
    }
    mudaPonteiros (aux, aux1);
}

void mudaPonteiros (struct nol *aux, struct nol *aux1){
    if (aux1->ant)
        aux1->ant->prox = aux1->prox;
    if (aux1->prox)
        aux1->prox->ant = aux1->ant;
    aux1->ant = aux;
    aux1->prox = aux->prox;
    if (aux->prox)
        aux->prox->ant = aux1;
    aux->prox = aux1;
} */