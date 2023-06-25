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
        fprintf (stderr, "Uso indevido, programa requer uma opção.\n");
        exit (1);
    }
    switch (comando){
        case 'i':
            if (argc < 4){
                fprintf (stderr, "Uso: vina++ -i <archive> [membro1 membro2 ...]\n");
                exit (1);
            }
            inclui (lista, argv, 'i');
        break;
        case 'a':
            if (argc < 4){
                fprintf (stderr, "Uso: vina++ -a <archive> [membro1 membro2 ...]\n");
                exit (1);
            }
            inclui (lista, argv, 'a');
        break;
        case 'm':
            if (argc != 5){
                fprintf (stderr, "Uso: vina++ -m <target> <archive> <file>\n");
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
                fprintf (stderr, "Uso: vina++ -r <archive> [membro1 membro2 ...]\n");
                exit (1);
            }
            exclui (lista, argv);
        break;
        case 'c':
            if (argc != 3){
                fprintf (stderr, "Uso: vina++ -c <archive>\n");
                exit (1);
            }
            imprimeInformacoes (lista, argv);
        break;
        case 'h':
            if (argc != 2){
                fprintf (stderr, "Uso: vina++ -h\n");
                exit (1);
            }
            imprimeOpcoes ();
        break;
        default:
            fprintf (stderr, "Opção inválida\n");
            exit(1);
    }
    destroiLista (lista);
    return 0;
}