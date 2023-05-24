#include "operacoes.h"
#include "util.h"

void inclui (struct lista *lista, char **args){
    FILE *arquivador;
    struct stat st;
    int pos = 0;
    char buffer[BUFFER];
    struct nol *aux;
    if (!(arquivador = fopen (args[2], "r+b")))
        arquivador = fopen (args[2], "w+b");
    extraiInformacoes (lista, arquivador);
    fwrite (&pos, sizeof(int), 1, arquivador);
    for (int i = 3; args[i] != NULL; i++){
        stat(args[i], &st);
        if ((aux = busca (args[i], lista))){
            atualizaNo (aux, st, arquivador);
            removeArquivo (aux, arquivador);
        }
        else
            adicionaNaCauda (lista, &st, args[i], fseek (arquivador, 0, SEEK_END));
        leArquivo (args[i], buffer, arquivador, st);
    }
    pos = ftell (arquivador);
    rewind (arquivador);
    fwrite (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, 0, SEEK_END);
    imprimeLista (lista, arquivador);
    fclose (arquivador);
}