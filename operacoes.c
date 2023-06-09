#include "operacoes.h"
#include "util.h"
#include <pwd.h>

void inclui (struct lista *lista, char **args, char modo){
    FILE *arquivador;
    struct stat st;
    long int diff = 0;
    char buffer[BUFFER], *nome;
    struct nol *aux;
    if (!(arquivador = fopen (args[2], "r+b"))){
        arquivador = fopen (args[2], "w+b");
        fwrite (&diff, sizeof (size_t), 1, arquivador);
    }
    else
        extraiInformacoes (lista, arquivador);
    for (int i = 3; args[i] != NULL; i++){
        stat(args[i], &st);
        nome = retornaNome (args[i]);
        if ((aux = busca (nome, lista))){
            if (modo == 'i' || st.st_mtime > aux->tempo){
                diff = st.st_size - aux->tamanho;
                refazEspaco (arquivador, aux, lista, diff);
                atualizaLista(-diff, aux->pos, lista);
                atualizaNo (aux, st, arquivador);
                fseek (arquivador, aux->pos, SEEK_SET);
                leArquivo (args[i], buffer, arquivador, st);
            }
        }
        else{
            fseek (arquivador, 0, SEEK_END);
            adicionaNaCauda (lista, &st, nome, ftell (arquivador));
            leArquivo (args[i], buffer, arquivador, st);
        }
        free (nome);
        nome = NULL;  
    }
    atualizaListaArchive (arquivador, lista);
}

void extrai (struct lista *lista, char **args){
    FILE *arquivador;
    struct nol *aux;
    char *nome;
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
                nome = retornaNome (args[i]);
                if ((aux = busca (nome, lista)))
                    extraiArquivo (aux, arquivador);
                else
                    printf ("Arquivo não encontrado.\n");
                free (nome);
                nome = NULL;
            }
        }
        fclose (arquivador);
    }
}

void exclui (struct lista *lista, char **args){
    FILE *arquivador;
    struct nol *aux, *aux1;
    char *nome;
    if (!(arquivador = fopen (args[2], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    else{
        extraiInformacoes (lista, arquivador);
        if (!args[3]){
            aux = lista->inicio;
            while (aux){
                removeArquivo (aux, arquivador, lista, 0);
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
                nome = retornaNome (args[i]);
                if ((aux = busca (nome, lista))){
                    removeArquivo (aux, arquivador, lista, 0);
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
            if (estaVazia (lista))
                remove (args[2]);
        }
        atualizaListaArchive (arquivador, lista);
    }
}

void move (struct lista *lista, char *target, char **args){
    FILE *arquivador;
    char *nomeTarget, *nomeMove;
    if (!(arquivador = fopen (args[3], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    struct nol *aux, *aux1;
    extraiInformacoes (lista, arquivador);
    nomeTarget = retornaNome (target);
    nomeMove = retornaNome (args[4]);
    aux = busca (nomeTarget, lista);
    aux1 = busca (nomeMove, lista);
    free (nomeTarget);
    free (nomeMove);
    if (!aux || !aux1){
        fprintf (stderr, "Arquivo inexistente\n");
        exit (1);
    }
    if (aux1 == aux->prox || aux1 == aux)
        return;
    abreEspaco (arquivador, aux1, aux, lista);
    if (aux1->pos > aux->pos){
        aux1->pos += aux1->tamanho;
        copiaArquivo (aux, aux1, arquivador);
        removeArquivo (aux1, arquivador, lista, 0);
        aux1->pos -= aux1->tamanho;
    }
    else{
        copiaArquivo (aux, aux1, arquivador);
        removeArquivo (aux1, arquivador, lista, aux1->tamanho);
    }
    atualizaMove (aux1, aux, lista);
    aux1->pos = aux->pos + aux->tamanho;
    mudaPonteiros (aux, aux1, lista);
    atualizaListaArchive (arquivador, lista);
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