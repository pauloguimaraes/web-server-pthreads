/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Aplicação client-side

Implementação baseada no tutorial: 
https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/



// Importando módulos necessários
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>



// Tamanho do buffer
#define MAX 256
// Porta de conexão
#define PORTA 8080
#define SOCKET_ADDRESS struct sockaddr


/**
 ** Método responsável pela ação do cliente após estabelecer comunicação com o servidor.
 **
 ** @param socket_fd: Socket de comunicação
 **/
void acao(int socket_fd) {
    char buffer[MAX];
    int n;

    for(;;) {
        // Zera o buffer
        bzero(buffer, sizeof(buffer));
        printf("Entre com o texto: ");
        
        // Lê o texto de entrada
        n = 0;
        while((buffer[n++] = getchar()) != '\n');

        // Escreve o texto no buffer e no socket
        write(socket_fd, buffer, sizeof(buffer));
        // Zera o buffer
        bzero(buffer, sizeof(buffer));

        // Lê a mensagem escrita no socket (resposta do servidor)
        read(socket_fd, buffer, sizeof(buffer));
        printf("\tDo servidor: %s", buffer);

        if((strncmp(buffer, "exit", 4)) == 0) {
            printf("Cliente saindo...\n");
            break;
        }
    }
}

/**
 ** Método de execução da aplicação
 **/
int main() {
    int socket_fd, connection_fd;
    struct sockaddr_in server_address, cli;

    // Inicia o socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        exit(0);
    }
    else
        printf("Socket criado...\n");

    // Zera o endereço do servidor
    bzero(&server_address, sizeof(server_address));

    // Atualiza informações do servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORTA);
    
    if(connect(socket_fd, (SOCKET_ADDRESS*)&server_address, sizeof(server_address)) != 0) {
        perror("Conexão falhou");
        printf("\n");
        exit(0);
    }
    else
        printf("Conectado ao servidor...\n");

    // Efetua a ação
    acao(socket_fd);
    // Fecha o socket
    close(socket_fd);
}