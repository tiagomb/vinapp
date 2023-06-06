#include "operacoes.h"
#include "util.h"
#include <pwd.h>

void inclui (struct lista *lista, char **args){
    FILE *arquivador;
    struct stat st;
    int pos = 0;
    long int diff;
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
            diff = st.st_size - aux->tamanho;
            refazEspaco (arquivador, aux, lista, diff);
            atualizaLista(-diff, aux->pos, lista);
            atualizaNo (aux, st, arquivador);
            fseek (arquivador, aux->pos, SEEK_SET);
        }
        else{
            fseek (arquivador, 0, SEEK_END);
            adicionaNaCauda (lista, &st, args[i], ftell (arquivador));
        }
        leArquivo (args[i], buffer, arquivador, st);
        fseek (arquivador, 0, SEEK_END);
        pos = ftell (arquivador);
        rewind (arquivador);
        fwrite (&pos, sizeof(int), 1, arquivador);
        fseek (arquivador, 0, SEEK_END);
    }
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
                removeArquivo (aux, arquivador, lista);
                atualizaLista(aux->tamanho, aux->pos, lista);
                aux1 = removeElemento (lista, aux->nome);
                free (aux1->nome);
                free (aux1);
                aux = aux->prox;
                pos = ftell (arquivador);
                rewind (arquivador);
                fwrite (&pos, sizeof(int), 1, arquivador);
                fseek (arquivador, 0, SEEK_END);
            }
            remove (args[2]);
        }
        else{
            for (int i = 3; args[i] != NULL; i++){
                if ((aux = busca (args[i], lista))){
                    removeArquivo (aux, arquivador, lista);
                    atualizaLista(aux->tamanho, aux->pos, lista);
                    aux1 = removeElemento (lista, aux->nome);
                    free (aux1->nome);
                    free (aux1);
                    pos = ftell (arquivador);
                    rewind (arquivador);
                    fwrite (&pos, sizeof(int), 1, arquivador);
                    fseek (arquivador, 0, SEEK_END);
                }
                else{
                    fprintf (stderr, "Arquivo inexistente\n");
                    exit (1);
                }
            }
            if (estaVazia (lista))
                remove (args[2]);
        }
        imprimeListaArq (lista, arquivador);
        fclose (arquivador);
    }
}

void move (struct lista *lista, char *target, char **args){
    FILE *arquivador;
    if (!(arquivador = fopen (args[3], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    struct nol *aux, *aux1;
    int pos;
    extraiInformacoes (lista, arquivador);
    aux = busca (target, lista);
    aux1 = busca (args[4], lista);
    if (!aux || !aux1){
        fprintf (stderr, "Arquivo inexistente\n");
        exit (1);
    }
    if (aux1 == aux->prox || aux1 == aux)
        return;
    abreEspaco (arquivador, aux1, aux, lista);
    if (aux1->pos > aux->pos){
        aux1->pos += aux1->tamanho;
    }
    copiaArquivo (aux, aux1, arquivador);
    removeArquivo (aux1, arquivador, lista);
    if (aux1->pos > aux->pos){
        aux1->pos -= aux1->tamanho;
    }
    atualizaMove (aux1, aux, lista);
    aux1->pos = aux->pos + aux->tamanho;
    //fazer funcao pra mudar nos
    fseek (arquivador, 0, SEEK_END);
    pos = ftell (arquivador);
    rewind (arquivador);
    fwrite (&pos, sizeof(int), 1, arquivador);
    fseek (arquivador, 0, SEEK_END);
    imprimeListaArq (lista, arquivador);
    fclose (arquivador);
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
        printf ("%ld\t", aux->tamanho);
        tm = localtime (&aux->tempo);
        printf ("%d-%02d-%02d %02d:%02d\t", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        printf ("%s\n", aux->nome);
        aux = aux->prox;
    }
    fclose (arquivador);
}

void imprimeOpcoes (){
    printf ("Usos:\n");
    printf ("vina++ -i <archive> [membro1 membro2 ...]\n");
    printf ("vina++ -a <archive> [membro1 membro2 ...]\n");
    printf ("vina++ -m <target> <archive> <file>\n");
    printf ("vina++ -x <archive> [membro1 membro2 ...]\n");
    printf ("vina++ -r <archive> [membro1 membro2 ...]\n");
    printf ("vina++ -c <archive>\n");
    printf ("vina++ -h\n");
}