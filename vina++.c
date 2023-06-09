#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lista.h"
#include "operacoes.h"

int main(int argc, char **argv){
    char *arquivo;
    int comando;
    struct lista *lista = inicializaLista();
    comando = getopt (argc, argv, "i:a:m:x:r:ch");
    if (comando == -1){
        fprintf (stderr, "Uso indevido\n");
        exit (1);
    }
    switch (comando){
        case 'i':
            inclui (lista, argv, 'i');
        break;
        case 'a':
            inclui (lista, argv, 'a');
        break;
        case 'm':
            arquivo = optarg;
            move (lista, arquivo, argv);
        break;
        case 'x':
            extrai (lista, argv);
        break;
        case 'r':
            exclui (lista, argv);
        break;
        case 'c':
            imprimeInformacoes (lista, argv);
        break;
        case 'h':
            imprimeOpcoes ();
        break;
        default:
            printf ("Uso indevido");
            exit(1);
    }
    destroiLista (lista);
    return 0;
}