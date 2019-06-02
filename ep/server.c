#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "pool_de_threads.h"

#define ENDERECO struct sockaddr

void envia_resposta(int _socket, char *resposta, int tamanho_resposta);
char *processa_requisicao(char *requisicao, int *tam_resposta);
void acao_da_thread(void *argumentos);


int main(int argc, char **argv) {
    int _socket, _conexao, tamanho;
    struct sockaddr_in endereco;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        exit(-1);
    }
    else
        printf("Socket criado.\n");

    bzero(&endereco, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco.sin_port = htons(8080);

    if((bind(_socket, (ENDERECO*)&endereco, sizeof(endereco))) != 0) {
        perror("Falha no bind");
        printf("\n");
        exit(-1);
    }
    else
        printf("Servidor pronto...\n");
    
    if((listen(_socket, 5)) != 0) {
        perror("Falha ao iniciar a escuta");
        printf("\n");
        exit(-1);
    }

    pool_de_threads _pool;
    _pool = cria_pool_de_threads(100);

    for(;;) {
        printf("laço principal\n");
        struct sockaddr_in _client;
        int tamanho = sizeof(_client);

        _conexao = accept(_socket, (ENDERECO*)&_client, &tamanho);
        if(_conexao < 0) {
            perror("Falha na aceitação do servidor");
            printf("\n");
            continue;
        }
        else
            printf("Conexão com cliente estabelecida\n");
        
        enviar(_pool, acao_da_thread, (void *)&_conexao);
    }
}

void envia_resposta(int _socket, char *resposta, int tamanho_resposta) {
    write(_socket, resposta, tamanho_resposta);
}

char *processa_requisicao(char *requisicao, int *tam_resposta) {
    char *resposta = (char *)malloc(1000 * sizeof(char));
    int i, j;

    for(i = 0; i<1000; i++)
        resposta[i] = requisicao[i%1000];
    
    printf("deu ruim no laço\n");
    resposta = "Funcionou, rapaz";
    
    sleep(15);

    *tam_resposta = 1000;
    printf("não deu ruim no laço\n");
    return resposta;
}

void acao_da_thread(void *argumentos) {
    char requisicao[1000];
    char *resposta = NULL;
    int _socket = *((int *) argumentos);

    bzero(requisicao, sizeof(requisicao));
    printf("Aqui entrou\n");

    read(_socket, requisicao, sizeof(requisicao));
    if(requisicao != NULL) {
        int tamanho_resp;

        printf("a lelek\n");
        resposta = processa_requisicao(requisicao, &tamanho_resp);
        if(resposta != NULL)
            envia_resposta(_socket, resposta, tamanho_resp);
    }
    bzero(requisicao, 1000);
    resposta = NULL;
    
    return;
}