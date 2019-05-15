/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Exemplo de aplicação usando PThreads

Implementação baseada no tutorial: 
https://www.thegeekstuff.com/2012/05/c-mutex-examples/?refcom
*/



// Importando módulos necessários
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// Lista de threads (conceito pode ser usado para criar as threads escravas)
pthread_t threads[2];
int contador;
pthread_mutex_t bloqueador;

/**
 ** Método responsável por incrementar o contador até metade do tamanho dos inteiros (ação que demora um pouco)
 **
 ** @param argumento: vazio
 **/
void* acao(void* argumento) {
    // Bloqueia o acesso ao trecho de código abaixo por outras threads
    pthread_mutex_lock(&bloqueador);
    printf("Bloqueado\n");

    printf("Começou a incrementar\n");
    int i = __INT_MAX__ / 2;

    while(i > 0) {
        contador++;
        i--;
    }

    printf("Incrementou\n");
    // Desbloqueia o acesso
    pthread_mutex_unlock(&bloqueador);
    printf("Desbloqueado\n");
    return NULL;
}

/**
 ** Método principal da aplicação
 **/
int main() {
    int i=0;

    // Inicia o mutex
    if(pthread_mutex_init(&bloqueador, NULL) != 0) {
        perror("Falha ao iniciar o mutex");
        return 1;
    }

    // Cria as duas threads
    while(i < 2) {
        printf("Criou a thread %d\n", i);
        int erro = pthread_create(&(threads[i]), NULL, &acao, NULL);

        if(erro != 0) {
            perror("Falha ao criar a thread");
            printf("\n");
        }

        i++;
    }

    // Espera elas terminarem
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    // Destroi o mutex
    pthread_mutex_destroy(&bloqueador);
    printf("%d\n", contador);
    return 0;
}