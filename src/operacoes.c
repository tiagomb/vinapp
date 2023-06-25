#include "operacoes.h"
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

/*A função abaixo é responsável por executar as operações de "-i" e "-a" especificadas no trabalho. Caso o arquivador não exista,
cria ele com o modo "w+", senão, abre com o modo "r+". A função itera pelos argumentos passados pelo usuário, fazendo as verificações
necessárias para saber se precisa inserir, atualizar ou não fazer nada.*/
void inclui (struct lista *lista, char **args, char modo){
    FILE *arquivador;
    struct stat st;
    long int diff = 0;
    char *nome;
    struct nol *aux;
    if (!(arquivador = fopen (args[2], "r+b"))){
        arquivador = fopen (args[2], "w+b");
        fwrite (&diff, sizeof (size_t), 1, arquivador);
    }
    else{
        extraiInformacoes (lista, arquivador);
        ftruncate (fileno (arquivador), lista->fim->pos + lista->fim->tamanho);
    }
    for (int i = 3; args[i] != NULL; i++){
        stat(args[i], &st);
        nome = retornaNome (args[i]);
        if ((aux = busca (nome, lista))){
            if (modo == 'i' || st.st_mtime > aux->tempo){
                diff = st.st_size - aux->tamanho;
                refazEspaco (arquivador, aux, lista, diff);
                atualizaLista(-diff, aux->ordem, lista, 0);
                atualizaNo (aux, st, arquivador);
                fseek (arquivador, aux->pos, SEEK_SET);
                leArquivo (args[i], arquivador, st);
            }
        }
        else{
            fseek (arquivador, 0, SEEK_END);
            adicionaNaCauda (lista, &st, nome, ftell (arquivador), i-2);
            leArquivo (args[i], arquivador, st);
        }
        free (nome);
        nome = NULL;  
    }
    atualizaListaArchive (arquivador, lista);
}

/*A função abaixo é responsável por extrair os arquivos passados como argumentos. Caso nenhum arquivo seja passado, todos são extráidos.
Caso o arquivo passado como argumentos tenha um caminho absoluto ou relativo, é chamada a função extraiDiretorio, que lida com a criação 
dos diretórios dentro do diretório atual*/
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
                if (aux->nome[0] == '.' && aux->nome[1] == '/')
                    extraiDiretorio (aux, arquivador);
                else
                    extraiArquivo (aux, arquivador, aux->nome);
                aux = aux->prox;
            }
        }
        else{
            for (int i = 3; args[i] != NULL; i++){
                nome = retornaNome (args[i]);
                if ((aux = busca (nome, lista))){
                    if (aux->nome[0] == '.' && aux->nome[1] == '/')
                        extraiDiretorio (aux, arquivador);
                    else
                        extraiArquivo (aux, arquivador, aux->nome);
                }
                else{
                    fprintf (stderr, "Arquivo %s inexistente\n", nome);
                    exit (1);
                }
                free (nome);
                nome = NULL;
            }
        }
        fclose (arquivador);
    }
}

/*A função abaixo exclui os arquivos passados como argumento pelo usuário. Caso ao final da remoção o arquivador esteja vazio, ele também é excluído.*/
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
        for (int i = 3; args[i] != NULL; i++){
            nome = retornaNome (args[i]);
            if ((aux = busca (nome, lista))){
                removeArquivo (aux, arquivador, lista, 0);
                atualizaLista(aux->tamanho, aux->ordem, lista, 1);
                aux1 = removeElemento (lista, aux->nome);
                free (aux1->nome);
                free (aux1);
            }
            else{
                fprintf (stderr, "Arquivo %s inexistente\n", nome);
                exit (1);
            }
            free (nome);
            nome = NULL;
        }
        if (estaVazia (lista))
            remove (args[2]);
    }
    atualizaListaArchive (arquivador, lista);
}

/*A função abaixo é responsável por mover o arquivo passado como argumento para logo após o arquivo target. Caso os arquivos sejam iguais
ou o arquivo a ser movido já esteja logo após target, nada acontece.*/
void move (struct lista *lista, char *target, char **args){
    FILE *arquivador;
    char *nomeTarget, *nomeMove;
    arquivador = fopen (args[3], "r+b");
    if (!arquivador){
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
        fprintf (stderr, "Falha ao encontrar arquivos\n");
        exit (1);
    }
    if (aux1 == aux->prox || aux1 == aux){
        fclose (arquivador);
        return;
    }
    abreEspaco (arquivador, aux1, aux, lista);
    if (aux1->ordem > aux->ordem){
        /*Caso aux1 esteja depois de aux, sua posição terá sido alterada na abertura de espaço, sendo necessário somar o
        seu tamanho para obter a posição real para cópia e remoção*/
        aux1->pos += aux1->tamanho;
        copiaArquivo (aux, aux1, arquivador);
        /*Como um espaço de aux1->tamanho foi aberto, o conteúdo do archive acaba aux1->tamanho á frente da posição salva na lista, sendo necessário
        um offset de aux1->tamanho para que a movimentação de dados na remoção do arquivo ocorra de forma correta*/
        removeArquivo (aux1, arquivador, lista, aux1->tamanho);
        aux1->pos -= aux1->tamanho; //retorna ao valor presente na lista para fazer as comparações necessárias com os outros nós e atualizá-los
    }
    else{
        copiaArquivo (aux, aux1, arquivador);
        removeArquivo (aux1, arquivador, lista, aux1->tamanho);
    }
    atualizaMove (aux1, aux, lista);
    aux1->pos = aux->pos + aux->tamanho;
    aux1->ordem = aux->ordem + 1;
    mudaPonteiros (aux, aux1, lista);
    atualizaListaArchive (arquivador, lista);
} 

/*A função abaixo é responsável por imprimir as informações presentes no diretório da maneira especificada no trabalho, ou seja, igual a opção
tar -tvf*/
void imprimeInformacoes (struct lista *lista, char **args){
    FILE *arquivador;
    if (!(arquivador = fopen (args[2], "r+b"))){
        fprintf (stderr, "Arquivador inexistente\n");
        exit (1);
    }
    struct nol *aux;
    struct passwd *pw;
    struct group *gr;
    struct tm *tm;
    extraiInformacoes (lista, arquivador);
    aux = lista->inicio;
    while (aux){
        imprimePermissoes (aux->perms);
        pw = getpwuid (aux->userid);
        gr = getgrgid (aux->groupid);
        printf ("%s/%s ", pw->pw_name, gr->gr_name);
        printf ("%8ld ", aux->tamanho);
        tm = localtime (&aux->tempo);
        printf ("%d-%02d-%02d %02d:%02d ", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        printf ("%s\n", aux->nome);
        aux = aux->prox;
    }
    fclose (arquivador);
}

/*A seguinte função é responsável por imprimir as opções possíveis de uso do programa para o usuário*/
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
