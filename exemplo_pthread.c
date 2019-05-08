/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Exemplo de aplicação usando PThreads
*/



// Importando módulos necessários
#include <pthread.h>
#include <stdio.h>



/**
 ** Método resposável por incrementar um valor de referência até 100
 **
 ** @param *x_void_pthread: Referência ao endereço do inteiro
 **/
void *incrementa_x(void *x_void_pthread) {
    int *x_pthread = (int *)x_void_pthread;
    
    while(++(*x_pthread) < 100);
    printf("X incrementado até 100\n");

    return NULL;
}

/**
 ** Método de execução da aplicação
 **/
int main() {
    int x = 0, y = 0;

    // Apresenta X e Y
    printf("X: %d, Y: %d\n", x, y);

    // Monta uma pthread
    pthread_t incrementa_x_thread;

    // Cria uma thread que executará o método incrementa_x
    if(pthread_create(&incrementa_x_thread, NULL, incrementa_x, &x)) {
        fprintf(stderr, "Erro ao criar a thread");
        return 1;
    }

    // Incrementa y até 100
    while(++y < 100);
    printf("Y incrementado até 100\n");

    // Aguarda até que a thread criada termine seu trabalho
    if(pthread_join(incrementa_x_thread, NULL)) {
        fprintf(stderr, "Erro esperando a thread");
        return 2;
    }

    // Apresenta X e Y
    printf("X: %d, Y: %d\n", x, y);
    return 0;
}