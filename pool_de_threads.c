/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Implementação do pool de threads escravas
*/



// Incluindo bibliotecas necessárias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "pool_de_threads.h"



/*
 * ---------------------------
 * -- Estruturas auxiliares --
 * ---------------------------
 */

// Tarefa a ser executada
typedef struct tarefa {
    void (*rotina) (void *);
    void *argumentos;
    struct cls_tarefa* proxima;
} cls_tarefa;

// Pool de threads
typedef struct threadpool {
    int qtd_disponivel;
    pthread_mutex_t bloqueador;
    pthread_cond_t vazia;
    pthread_cond_t nao_vazia;
    pthread_t *threads;

    int tamanho_thread;
    cls_tarefa *thread;
    cls_tarefa *prox_thread;
} cls_threadpool;



/*
 * -------------
 * -- Métodos --
 * -------------
 */

/**
 ** Método que cria um pool de threads.
 **
 ** @param numero_de_threads: Número de threads que pool terá.
 **/
pool_de_threads cria_pool_de_threads(int numero_de_threads) {
    cls_threadpool *_pool;
    int i;

    // Se deseja criar com quantidade negativa ou maior que o limite máximo não faz nada
    if((numero_de_threads <= 0) || (numero_de_threads > NUMERO_MAXIMO_THREADS))
        return NULL;

    // Aloca a memória necessária
    _pool = (cls_threadpool *)malloc(sizeof(cls_threadpool));
    if(_pool == NULL){
        ferror("Erro na criação do pool");
        printf("\n");
        return NULL;
    }
    else
        printf("Memória alocada para o pool de threads.\n");

    // Configura a classe de pool de threads
    _pool->qtd_disponivel = numero_de_threads;
    _pool->tamanho_thread = 0;
    _pool->thread = NULL;
    _pool->prox_thread = NULL;
    _pool->threads = (pthread_t *)malloc(numero_de_threads * sizeof(pthread_t));
    // Inicia o bloqueador
    pthread_mutex_init(&(_pool->bloqueador), NULL);
    // Inicia os condicionadores do pool de threads
    pthread_cond_init(&(_pool->vazia), NULL);
    pthread_cond_init(&(_pool->nao_vazia), NULL);

    // Cria as threads
    for(i = 0; i < _pool->qtd_disponivel; i++)
        if(pthread_create(&(_pool->threads[i]), NULL, trabalho_thread, _pool) != 0) {
            ferror("Erro criando thread");
            printf("\n");
            return NULL;
        }
        else
            printf("Thread %d criada.\n", i);
    
    return (pool_de_threads)_pool;
}

/**
 ** Método que delega um trabalho para o pool, destinando a primeira thread disponível. 
 ** 
 ** @param pool: Pool de threads que fará o trabalho.
 **/
void *trabalho_thread(pool_de_threads pool) {
    cls_threadpool *_pool = (cls_threadpool *)pool;
    cls_tarefa *tarefa_atual;

    // Efetua a operação bloqueante
    pthread_mutex_lock(&(_pool->bloqueador));
    printf("Bloqueador ligado aguardar tarefa.\n");

    for(;;) {
        // Aguarda até existir threads disponíveis
        while(_pool->tamanho_thread == 0)
            pthread_cond_wait(&(_pool->nao_vazia), &(_pool->bloqueador));

        printf("Existem threads disponíveis.\n");
        tarefa_atual = _pool->thread;
        _pool->tamanho_thread--;

        if(_pool->tamanho_thread == 0) {
            _pool->thread = NULL;
            _pool->prox_thread = NULL;
        }
        else
            _pool->thread = tarefa_atual->proxima;
        
        pthread_mutex_unlock(&(_pool->bloqueador));
        printf("Bloqueador desligado após receber tarefa.\n");
        (tarefa_atual->rotina) (tarefa_atual->argumentos);

        free(tarefa_atual);        
    }

    // Desbloqueia
    pthread_mutex_unlock(&(_pool->bloqueador));
    printf("Bloqueador desligado após receber tarefa.\n");
}

/**
 ** Adiciona uma função para ser executada pelo pool de threads.
 **
 ** @param pool: Pool de threads que executará a função.
 ** @param funcao_de_envio: Função que será executada pelo pool.
 ** @param argumentos: Parâmetros que serão enviados para a função.
 **/
void adiciona_tarefa_no_pool(pool_de_threads pool, envio funcao_de_envio, void *argumentos) {
    cls_threadpool *_pool = (cls_threadpool *)pool;
    cls_tarefa *tarefa;

    // Aloca espaço para a tarefa
    tarefa = (cls_tarefa *)malloc(sizeof(cls_tarefa));
    if(tarefa == NULL) {
        ferror("Erro alocando espaço");
        return;
    }
    else
        printf("Alocou espaço para executar a tarefa.\n");

    // Define a rotina para ser executada
    tarefa->rotina = funcao_de_envio;
    tarefa->argumentos = argumentos;
    tarefa->proxima = NULL;

    // Bloqueia em trecho crítico
    pthread_mutex_lock(&(_pool->bloqueador));
    printf("Bloqueador ligado para manipular o pool.\n");

    if(_pool->tamanho_thread == 0) {
        _pool->tamanho_thread++;
        _pool->thread = tarefa;
        _pool->prox_thread = tarefa;
        pthread_cond_signal(&(_pool->nao_vazia));
    }
    else {
        _pool->tamanho_thread++;
        _pool->prox_thread->proxima = tarefa;
        _pool->prox_thread = tarefa;
        pthread_cond_signal(&(_pool->nao_vazia));
    }

    pthread_mutex_unlock(&(_pool->bloqueador));
    printf("Bloquador desligado após manipular o pool.\n");
}

/**
 ** Método que encerra o pool. 
 ** 
 ** @pool_para_encerrar: Pool que será encerrado.
 **/
void encerra_pool(pool_de_threads pool_para_encerrar) {
    cls_threadpool *_pool = (cls_threadpool *)pool_para_encerrar;

    pthread_mutex_destroy(&(_pool->bloqueador));
    pthread_cond_destroy(&(_pool->vazia));
    pthread_cond_destroy(&(_pool->nao_vazia));
    _pool->qtd_disponivel = 0;
    free(_pool->threads);
    free(_pool);

    return;
}