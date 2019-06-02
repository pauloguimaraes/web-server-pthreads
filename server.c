/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Implementação do servidor web
*/



// Incluindo bibliotecas necessárias
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "pool_de_threads.h"
#include "server.h"
#include "util.h"

// Constantes
#define ENDERECO struct sockaddr



/*
 * -------------
 * -- Métodos --
 * -------------
 */

/**
 ** Método principal da aplicação.
 **
 ** @param argc: Quantidade de argumentos.
 ** @param argv: Argumentos enviados.
 **/
int main(int argc, char **argv) {
    int _socket, _conexao, tamanho;
    struct sockaddr_in endereco;

    // Criação do socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        exit(-1);
    }
    else
        printf("Socket criado.\n");

    // Cofingurando o socket
    bzero(&endereco, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco.sin_port = htons(PORTA);

    // Atrela o socket
    if((bind(_socket, (ENDERECO*)&endereco, sizeof(endereco))) != 0) {
        perror("Falha no bind");
        printf("\n");
        exit(-1);
    }
    else
        printf("Servidor pronto.\n");
    
    // Faz o servidor iniciar a escuta
    if((listen(_socket, 5)) != 0) {
        perror("Falha ao iniciar a escuta");
        printf("\n");
        exit(-1);
    }
    else
        printf("Servidor aguardando.\n");

    // Inicia o pool de threads
    pool_de_threads _pool;
    _pool = cria_pool_de_threads(NUM_THREADS_REGULAR);


    // Laço infinito para conexão com clientes
    for(;;) {
        struct sockaddr_in _client;
        int tamanho = sizeof(_client);

        // Recebe a conexão do cliente
        _conexao = accept(_socket, (ENDERECO*)&_client, &tamanho);
        if(_conexao < 0) {
            perror("Falha na aceitação do servidor");
            printf("\n");
            continue;
        }
        else
            printf("Conexão com cliente estabelecida\n");
        
        // Adiciona a requisição do cliente no pool de threads
        adiciona_tarefa_no_pool(_pool, acao_da_thread, (void *)&_conexao);
    }

    // Mata o pool de threads
    encerra_pool(_pool);
}

/**
 ** Método que processa a requisição enviada pelo cliente.
 **
 ** @param requisicao: Requisição enviada.
 **/
char *processa_requisicao(char *requisicao) {
    // Alocando espaço suficiente para resposta
    char *resposta = (char *)malloc(TAMANHO_REQUISICAO * sizeof(char));
    
    printf("Recebemos a mensagem: %s", requisicao);
    resposta = "Infelizmente não sei te responder...";
    
    printf("Simulando um processamento, aguardaremos 15 segundos...\n");
    sleep(15);

    return resposta;
}

/**
 ** Método que delega a ação de processamento para o pool de threads.
 **
 ** @param argumentos: Socket que recebeu a requisição.
 **/
void acao_da_thread(void *argumentos) {
    char requisicao[TAMANHO_REQUISICAO];
    char *resposta = NULL;
    int _socket = *((int *) argumentos);

    bzero(requisicao, sizeof(requisicao));

    // Lê o que foi escrito no socket
    read(_socket, requisicao, sizeof(requisicao));
    if(requisicao != NULL) {
        // Processa a requisição
        resposta = processa_requisicao(requisicao);

        // Se obteve resposta, envia para o cliente.
        if(resposta != NULL) {
            write(_socket, resposta, TAMANHO_REQUISICAO);
            printf("Resposta enviada ao cliente.\n");
        }
    }
    bzero(requisicao, sizeof(requisicao));
    resposta = NULL;
    
    return;
}