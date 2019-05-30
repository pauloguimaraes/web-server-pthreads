// Importa as bibliotecas necessárias
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>



/**
 ** Variáveis
 **/

// Status das threads
#define AGUARDANDO 0
#define EM_USO 1
#define ERRO 2

// Variáveis auxiliares
#define MAX 256
#define PORTA 8080
#define ENDERECO_SOCKET struct sockaddr

// Threads e bloqueador
pthread_t threads_filhas[100];
pthread_mutex_t bloqueador;

// Arranjo para controle de status das threads
int indicador_uso_thread[100];



/**
 ** Métodos 
 **/

/**
 ** Método que efetua o trabalho de uma thread.
 **
 ** @param argumento: possível argumento do método.
 **/
void* trabalho_das_threads(void *argumento) {
    return NULL;
}

/**
 ** Método que controle a zona crítica da aplicação.
 **
 ** @param argumento: possível argumento do método.
 **/
void* entra_em_trecho_critico(void* argumento) {
    pthread_mutex_lock(&bloqueador);
    printf("Início do bloqueio...\n");

    pthread_mutex_unlock(&bloqueador);
    printf("Final do bloqueio.\n");
    return NULL;
}

/**
 ** Método que encerra a atividade do servidor.
 ** 
 ** @param _socket: Socket que será encerrado.
 **/
int encerra_servidor(int _socket) {
    close(_socket);
    printf("Socket encerrado.\n");
    return 0;
}

/**
 ** Método que encerra um bloqueador.
 ** 
 ** @param bloq: Bloqueador que está sendo encerrado.
 **/
int encerra_bloqueador(pthread_mutex_t bloq) {
    pthread_mutex_destroy(&bloq);
    printf("Bloqueador encerrado.\n");
    return 0;
} 

/**
 ** Método que inicia os trabalhos do servidor.
 ** 
 ** @param _socket: Socket para troca de mensagens com o cliente.
 ** @param tamanho: tamanho das mensagens que serão trocadas.
 ** @param conexao: Definição de conexão estabelecida com o cliente.
 ** @param endereco: Endereço do servidor
 ** @param cliente: Cliente que estamos nos comunicando.
 **/
int inicia_servidor(int _socket, int tamanho, int conexao, struct sockaddr_in endereco, struct sockaddr_in cliente) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        perror("Falha na criação do socket");
        printf("\n");
        return 1;
    } else {
        printf("Socket criado.\n");
    }

    bzero(&endereco, sizeof(endereco));

    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco.sin_port = htons(PORTA);

    if((bind(_socket, (ENDERECO_SOCKET*) &endereco, sizeof(endereco))) != 0) {
        perror("Falha no bind do socket");
        printf("\n");
        return 1;
    } else {
        printf("Servidor aguardando...\n");
    }
    
    // Começa a escutar //adicionado por Pipa, n tenho ctz se tá certo e se deve ser assim
    if((listen(socket_fd, 5)) != 0) {
        perror("Falha ao iniciar a escuta");
        printf("\n");
        exit(0);
    }
    else
        printf("Servidor ouvindo...\n");
    

    tamanho = sizeof(cliente);
    conexao = accept(_socket, (ENDERECO_SOCKET*)&cliente, &tamanho);
    if(conexao < 0) {
        perror("Falha na conexão");
        printf("\n");
        return 1;
    }
    else {
        printf("Conexão estabelecida");
    }
    return 0;
}

/**
 ** Método que inicia um bloqueador.
 ** 
 ** @param bloq: Bloqueador que está sendo iniciado.
 **/
int inicia_bloqueador(pthread_mutex_t bloq) {
    if(pthread_mutex_init(&bloq, NULL) != 0) {
        perror("Falha ao iniciar o mutex");
        return 1;
    }
    return 0;
}

/**
 ** Inicia as threads.
 **
 ** @param threads: Arranjo de threads que serão iniciadas 
 **/
int inicia_threads(pthread_t threads[]) {
    int qtd_threads = sizeof(threads) / sizeof(threads[0]);

    int i = 0;
    while(i < qtd_threads) {
        printf("Criou a thread %d\n", i);
        indicador_uso_thread[i] = AGUARDANDO;

        int erro = pthread_create(&(threads[i]), NULL, &trabalho_das_threads, NULL);

        if(erro != 0) {
            perror("Falha ao criar a thread");
            printf("\n");
            return 1;
        }

        i++;
    }
    return 0;
}

/**
 ** Método principal da aplicação
 **/
int main() {
    int _socket, conexao, tamanho;
    struct sockaddr_in endereco, cliente;

    int resposta;

    // Inicia a aplicação como servidor
    resposta = inicia_servidor(_socket, tamanho, conexao, endereco, cliente);
    if(resposta > 0)
        exit(1);

    // Inicia o bloqueador
    resposta = inicia_bloqueador(bloqueador);
    if(resposta > 0)
        exit(1);

    // Inicia as threads
    resposta = inicia_threads(threads_filhas);
    if(resposta > 0)
        exit(1);

    // Encerra o bloqueador
    encerra_bloqueador(&bloqueador);
    // Encerra o servidor
    encerra_servidor(_socket);
} 
