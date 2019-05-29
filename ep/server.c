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

// Cria sinônimos
#define Thread pthread_t;
#define Mutex pthread_mutex_t;

// Status das threads
#define AGUARDANDO 0
#define EM_USO 1
#define ERRO 2

// Variáveis auxiliares
#define MAX 256
#define PORTA 8080
#define ENDERECO_SOCKET struct sockaddr

// Threads e bloqueador
Thread threads_filhas[100];
Mutex bloqueador;

// Arranjo para controle de status das threads
int indicador_uso_thread[100];

// Arquivo de log
FILE *log;

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
    fprintf(log, "Início do bloqueio...\n");

    pthread_mutex_unlock(&bloqueador);
    fprintf(log, "Final do bloqueio.\n");
    return NULL;
}

/**
 ** Método que encerra a atividade do servidor.
 ** 
 ** @param _socket: Socket que será encerrado.
 **/
int encerra_servidor(int _socket) {
    close(_socket);
    fprintf(log, "Socket encerrado.\n");
}

/**
 ** Método que encerra um bloqueador.
 ** 
 ** @param bloq: Bloqueador que está sendo encerrado.
 **/
int encerra_bloqueador(pthread_mutex_t bloq) {
    pthread_mutex_destroy(&bloq);
    fprintf(log, "Bloqueador encerrado.\n");
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
        fprintf(log, "Socket criado.\n");
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
        fprintf(log, "Servidor aguardando...\n");
    }

    tamanho = sizeof(cliente);
    conexao = accept(_socket, (ENDERECO_SOCKET*)&cliente, &tamanho);
    if(conexao < 0) {
        perror("Falha na conexão");
        printf("\n");
        return 1;
    }
    else {
        fprintf(log, "Conexão estabelecida");
    }
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
        fprintf(log, "Criou a thread %d\n", i);
        indicador_uso_thread[i] = AGUARDANDO;

        int erro = pthread_create(&(threads[i]), NULL, &trabalho_das_threads, NULL);

        if(erro != 0) {
            perror("Falha ao criar a thread");
            printf("\n");
            return 1;
        }

        i++;
    }
}

/**
 ** Cria, abre e inicializa o arquivo de log.
 **/
void cria_arquivo_log(){
	log = fopen("server_log_8094403_9390361.txt", "w");
	fprintf(log, "Arquivo de log do Exercício Programa 1 de DSID - Professora Gisele\n");
	fprintf(log, "Trabalho dos alunos Lucas Pipa Cervera (nusp 8094403) e Paulo Henrique Freitas Guimarães (nusp 9390361)\n");
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
    
    fclose(log);
}
