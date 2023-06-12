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
            if (argc < 4){
                fprintf (stderr, "Uso indevido\n");
                exit (1);
            }
            inclui (lista, argv, 'i');
        break;
        case 'a':
            if (argc < 4){
                fprintf (stderr, "Uso indevido\n");
                exit (1);
            }
            inclui (lista, argv, 'a');
        break;
        case 'm':
            if (argc != 5){
                fprintf (stderr, "Uso indevido\n");
                exit (1);
            }
            arquivo = optarg;
            move (lista, arquivo, argv);
        break;
        case 'x':
            extrai (lista, argv);
        break;
        case 'r':
            if (argc < 4){
                fprintf (stderr, "Uso indevido\n");
                exit (1);
            }
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