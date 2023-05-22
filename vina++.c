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
            inclui (lista, argv);
        break;
        case 'a':
            printf ("%d argumentos para atualizar em %s\n", argc - 3, argv[2]);
        break;
        case 'm':
            arquivo = optarg;
            printf ("Mover %s\n", arquivo);
        break;
        case 'x':
            printf ("%d argumentos para excluir em %s\n", argc - 3, argv[2]);
        break;
        case 'c':
            printf ("Listar conteudo\n");
        break;
        case 'h':
            printf ("Imprimir ajuda\n");
        break;
        default:
            printf ("Uso indevido");
            exit(1);
    }
    destroiLista (lista);
    return 0;
}