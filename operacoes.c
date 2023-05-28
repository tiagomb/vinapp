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
    else
        extraiInformacoes (lista, arquivador);
    fwrite (&pos, sizeof(int), 1, arquivador);
    for (int i = 3; args[i] != NULL; i++){
        stat(args[i], &st);
        if ((aux = busca (args[i], lista))){
            removeArquivo (aux, arquivador);
            atualizaNo (aux, st, arquivador);
        }
        else
            adicionaNaCauda (lista, &st, args[i], ftell (arquivador));
        leArquivo (args[i], buffer, arquivador, st);
    }
    pos = ftell (arquivador);
    rewind (arquivador);
    fwrite (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, 0, SEEK_END);
    imprimeListaArq (lista, arquivador);
    fclose (arquivador);
}

void extrai (struct lista *lista, char **args){
    FILE *arquivador;
    struct nol *aux;
    if (!(arquivador = fopen (args[2], "rb"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    else{
        extraiInformacoes (lista, arquivador);
        if (!args[3]){
            aux = lista->inicio;
            while (aux){
                extraiArquivo (aux, arquivador);
                aux = aux->prox;
            }
        }
        else{
            for (int i = 3; args[i] != NULL; i++){
                if ((aux = busca (args[i], lista)))
                    extraiArquivo (aux, arquivador);
                else
                    printf ("Arquivo não encontrado.\n");
            }
        }
        fclose (arquivador);
    }
}
