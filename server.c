/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Aplicação server-side
*/



// Importando módulos necessários
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>



// Tamanho do buffer
#define MAX 80
// Porta de conexão
#define PORTA 8080
#define SOCKET_ADDRESS struct sockaddr



/**
 ** Método responsável pela ação do servidor após estabelecer comunicação com um cliente.
 **
 ** @param socket_fd: Socket de comunicação
 **/
void acao(int socket_fd) {
    char buffer[MAX];
    int n;

    for(;;) {
        // Zera o buffer
        bzero(buffer, MAX);

        // Lê o que o cliente enviou
        read(socket_fd, buffer, sizeof(buffer));
        printf("Do cliente: %s\t To client : ", buffer);
        // Zera o buffer
        bzero(buffer, MAX);

        // Lê a entrada do servidor
        n = 0;
        while((buffer[n++] = getchar()) != '\n');

        // Escreve a entrada do servidor no socket
        write(socket_fd, buffer, sizeof(buffer));

        if(strncmp("exit", buffer, 4) == 0) {
            printf("Servidor parando...\n");
            break;
        }
    }
}

/**
 ** Método de execução da aplicação
 **/
int main() {
    int socket_fd, connection_fd, tamanho;
    struct sockaddr_in server_address, cli;

    // Inicia o socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        printf("Falha na criação do socket...\n");
        exit(0);
    }
    else
        printf("Socket criado...\n");

    // Zera o endereço do servidor
    bzero(&server_address, sizeof(server_address));

    // Atualiza as informações do servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORTA);

    // Faz o bind
    if((bind(socket_fd, (SOCKET_ADDRESS*)&server_address, sizeof(server_address))) != 0) {
        printf("Falha no bind do socket...\n");
        exit(0);
    }
    else
        printf("Socket bindado...\n");

    // Começa a escutar
    if((listen(socket_fd, 5)) != 0) {
        printf("Falha no listen...\n");
        exit(0);
    }
    else
        printf("Servidor listening...\n");

    // Estabele a conexão com o cliente
    tamanho = sizeof(cli);
    connection_fd = accept(socket_fd, (SOCKET_ADDRESS*)&cli, &tamanho);
    if(connection_fd < 0) {
        printf("Falha na aceitação do servidor...\n");
        exit(0);
    }
    else
        printf("Servidor aceitou o cliente...\n");

    // Executa a ação
    acao(connection_fd);
    // Fecha o socket
    close(socket_fd);
}