#include "caminho.h"
#include "diretorio.h"
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>

/*A seguinte funcao retorna o nome do arquivo conforme especificado pelo enunciado. Caso o nome comece com uma barra (caminho absoluto), é inserido
um '.' no inicio da string. Caso haja barra no meio do nome (caminho relativo), é inserido um './' no comeco da string. Caso contrário, a string é retornada
do mesmo jeito que foi passada como parametro*/
char *retornaNome (char *path){
    char *aux;
    if (path[0] == '/'){
        aux = malloc (sizeof (char) * (strlen (path) + 2));
        if (!aux){
            fprintf (stderr, "Erro na alocacao de memoria\n");
            exit (1);
        }
        aux[0] = '.';
        aux += 1;
        strncpy (aux, path, strlen (path));
        aux[strlen (path)] = '\0';
        aux -= 1;
        return aux;
    }
    else{
        for (int i = 1; i < strlen (path); i++){
            if (path[i] == '/'){
                aux = malloc (sizeof (char) * (strlen (path) + 3));
                if (!aux){
                    fprintf (stderr, "Erro na alocacao de memoria\n");
                    exit (1);
                }
                aux[0] = '.';
                aux[1] = '/';
                aux += 2;
                strncpy (aux, path, strlen (path));
                aux[strlen (path)] = '\0';
                aux -= 2;
                return aux;
            }
        }
        aux = malloc (sizeof (char) * (strlen (path) + 1));
        if (!aux){
            fprintf (stderr, "Erro na alocacao de memoria\n");
            exit (1);
        }
        strncpy (aux, path, strlen (path));
        aux[strlen (path)] = '\0';
        return aux;
    }
}

/*A seguinte funcao recebe um caminho e retorna o nome do arquivo em si. Ou seja, dado um nome "./home/inf/xyz00/abc.txt",
a funcao retorna "abc.txt"*/
char *retornaNomeArq (char *path){
    int tam = 0, i = strlen (path) - 1;
    while (path[i] != '/'){
        tam++;
        i--;
    }
    char *aux = malloc (sizeof (char) * (tam + 1));
    for (int j = 0; j < tam; j++){
        aux[j] = path[i+1];
        i++;
    }
    aux[tam] = '\0';
    return aux;
}

/*A funcao abaixo cria os diretorios presentes no caminho de um arquivo dentro do diretorio atual e ao final extrai o 
arquivo dentro desses diretorios.*/
void extraiDiretorio (struct nol *no, FILE *arquivador){
    char *nomeArq, *aux;
    char path[BUFFER];
    getcwd (path, BUFFER);
    nomeArq = retornaNomeArq (no->nome);
    no->nome += 2;
    aux = strtok (no->nome, "/");
    no->nome -= 2;
    while (strcmp (aux, nomeArq)){
        mkdir (aux, no->perms | S_IRWXU);
        chdir (aux);
        aux = strtok (NULL, "/");
    }
    extraiArquivo (no, arquivador, nomeArq);
    chdir (path);
    free (nomeArq);
}
