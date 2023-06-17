#include "caminho.h"
#include "diretorio.h"
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>

char *retornaNome (char *path){
    char *aux;
    if (path[0] == '/'){
        aux = malloc (sizeof (char) * (strlen (path) + 2));
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
        strncpy (aux, path, strlen (path));
        aux[strlen (path)] = '\0';
        return aux;
    }
}

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