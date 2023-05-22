#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

char **le_args (int argc, char**argv){
    char **aux;
    int i = 0;
    if (argc < 3)
        return NULL;
    aux = malloc (sizeof(char) * (argc - 2));
    while (i < argc - 2){
        aux[i] = argv[i + 2];
        i++;
    }
    return aux;
}

int main(int argc, char **argv){
    char **args, *arquivo;
    int comando;
    comando = getopt (argc, argv, "i:a:m:x:r:ch");
    if (comando == -1){
        fprintf (stderr, "Uso indevido\n");
        exit (1);
    }
    switch (comando){
        case 'i':
            args = le_args (argc, argv);
            printf ("%d argumentos para incluir em %s\n", argc - 3, args[0]);
        break;
        case 'a':
            args = le_args (argc, argv);
            printf ("%d argumentos para atualizar em %s\n", argc - 3, args[0]);
        break;
        case 'm':
            arquivo = optarg;
            printf ("Mover %s\n", arquivo);
        break;
        case 'x':
            args = le_args (argc, argv);
            printf ("%d argumentos para excluir em %s\n", argc - 3, args[0]);
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
    return 0;
}