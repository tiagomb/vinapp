#include "util.h"

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

void removeArquivo (struct nol *no, FILE *arquivador){
}