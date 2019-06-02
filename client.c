/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Implementação da aplicação cliente
*/



// Incluindo bibliotecas necessárias
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "util.h"

// Constantes
#define ENDERECO struct sockaddr



/*
 * -------------
 * -- Métodos --
 * -------------
 */

/**
 ** Método que efetua as ações do cliente.
 **
 ** @param _socket: Socket de comunicação.
 **/
void envia_requisicao(int _socket) {
    char buffer[TAMANHO_REQUISICAO];
    int n;

    // Lê a entrada
    bzero(buffer, sizeof(buffer));
    printf("Entre com o texto: ");

    n = 0;
    while((buffer[n++] = getchar()) != '\n');

    // Escreve no socket
    write(_socket, buffer, sizeof(buffer));
    bzero(buffer, sizeof(buffer));

    // Lê a resposta
    read(_socket, buffer, sizeof(buffer));
    // Apresenta
    printf("\tDo servidor: %s", buffer);
    printf("\n");
}

/**
 ** Método principal da aplicação.
 **
 ** @param argc: Quantidade de argumentos.
 ** @param argv: Argumentos enviados.
 **/
int main(int argc, char **argv) {
    int _socket, i;
    struct sockaddr_in _endereco, _client;

    // Inicia o socket
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        exit(-1);
    }
    else
        printf("Socket criado...\n");

    // Monta o endereço
    bzero(&_endereco, sizeof(_endereco));
    _endereco.sin_family = AF_INET;
    _endereco.sin_addr.s_addr = inet_addr("127.0.0.1");
    _endereco.sin_port = htons(8080);

    // Conecta com o servidor
    if(connect(_socket, (ENDERECO*)&_endereco, sizeof(_endereco)) != 0) {
        perror("Conexão falhou");
        printf("\n");
        exit(-1);
    }
    else
        printf("Conectado ao servidor...\n");

    // Envia a requisição
    envia_requisicao(_socket);
    // Encerra o socket
    close(_socket);
}