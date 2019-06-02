#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define ENDERECO struct sockaddr

void acao(int _socket) {
    char buffer[1000];
    int n;

    bzero(buffer, sizeof(buffer));
    printf("Entre com o texto: ");

    n = 0;
    while((buffer[n++] = getchar()) != '\n');

    write(_socket, buffer, sizeof(buffer));
    bzero(buffer, sizeof(buffer));

    read(_socket, buffer, sizeof(buffer));
    printf("\tDo servidor: %s", buffer);
    printf("\n");

    if((strncmp(buffer, "exit", 4)) == 0) {
        printf("Cliente saindo...\n");
        return(0);
    }
}

int main(int argc, char **argv) {
    int _socket, i;
    struct sockaddr_in _endereco, _client;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        exit(-1);
    }
    else
        printf("Socket criado...\n");

    bzero(&_endereco, sizeof(_endereco));

    _endereco.sin_family = AF_INET;
    _endereco.sin_addr.s_addr = inet_addr("127.0.0.1");
    _endereco.sin_port = htons(8080);

    if(connect(_socket, (ENDERECO*)&_endereco, sizeof(_endereco)) != 0) {
        perror("Conexão falhou");
        printf("\n");
        exit(-1);
    }
    else
        printf("Conectado ao servidor...\n");

    acao(_socket);
    close(_socket);
}