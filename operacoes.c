#include "operacoes.h"
#include "util.h"
#include <pwd.h>

void inclui (struct lista *lista, char **args){
    FILE *arquivador;
    struct stat st;
    int pos = 0;
    char buffer[BUFFER];
    struct nol *aux;
    if (!(arquivador = fopen (args[2], "r+b"))){
        arquivador = fopen (args[2], "w+b");
        fwrite (&pos, sizeof(int), 1, arquivador);
    }
    else
        extraiInformacoes (lista, arquivador);
    for (int i = 3; args[i] != NULL; i++){
        stat(args[i], &st);
        if ((aux = busca (args[i], lista))){
            removeArquivo (aux, arquivador);
            atualizaLista(aux->tamanho, aux->pos, lista);
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

void exclui (struct lista *lista, char **args){
    FILE *arquivador;
    struct nol *aux, *aux1;
    int pos;
    if (!(arquivador = fopen (args[2], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    else{
        extraiInformacoes (lista, arquivador);
        if (!args[3]){
            aux = lista->inicio;
            while (aux){
                removeArquivo (aux, arquivador);
                atualizaLista(aux->tamanho, aux->pos, lista);
                aux1 = removeElemento (lista, aux->nome);
                free (aux1->nome);
                free (aux1);
                aux = aux->prox;
            }
            remove (args[2]);
        }
        else{
            for (int i = 3; args[i] != NULL; i++){
                if ((aux = busca (args[i], lista))){
                    removeArquivo (aux, arquivador);
                    atualizaLista(aux->tamanho, aux->pos, lista);
                    aux1 = removeElemento (lista, aux->nome);
                    free (aux1->nome);
                    free (aux1);
                }
                else{
                    fprintf (stderr, "Arquivo inexistente\n");
                    exit (1);
                }
            }
        }
        pos = ftell (arquivador);
        rewind (arquivador);
        fwrite (&pos, sizeof(int), 1, arquivador);
        fseek (arquivador, 0, SEEK_END);
        imprimeListaArq (lista, arquivador);
        fclose (arquivador);
    }
}

void imprimeInformacoes (struct lista *lista, char **args){
    FILE *arquivador;
    if (!(arquivador = fopen (args[2], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    struct nol *aux;
    struct passwd *pw;
    struct tm *tm;
    extraiInformacoes (lista, arquivador);
    aux = lista->inicio;
    while (aux){
        imprimePermissoes (aux->perms);
        pw = getpwuid (aux->userid);
        printf ("%s\t", pw->pw_name);
        printf ("%lld\t", aux->tamanho);
        tm = localtime (&aux->tempo);
        printf ("%d-%02d-%02d %02d:%02d ", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        printf ("%s\n", aux->nome);
        aux = aux->prox;
    }
    fclose (arquivador);
}