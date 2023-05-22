#include "operacoes.h"

void inclui (struct lista *lista, char **args){
    FILE *arquivador, *arquivo;
    struct stat st;
    int leituras, pos = 0;
    char buffer[BUFFER];
    if (!(arquivador = fopen(args[2], "r"))){
        arquivador = fopen (args[2], "wb+");
        fwrite (&pos, sizeof(int), 1, arquivador);
        for (int i = 3; args[i] != NULL; i++){
            stat(args[i], &st);
            adicionaNaCauda (lista, &st, args[i], i-3, fseek (arquivador, 0, SEEK_END));
            if (!(arquivo = fopen (args[i], "r"))){
                fprintf (stderr, "Falha ao abrir %s\n", args[i]);
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
        }
        pos = ftell (arquivador);
        rewind (arquivador);
        fwrite (&pos, sizeof(int), 1, arquivador);
        fseek (arquivador, 0, SEEK_END);
        imprimeLista (lista, arquivador);
    }
}