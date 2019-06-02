#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "pool_de_threads.h"

typedef struct tarefa {
    void (*rotina) (void *);
    void *argumentos;
    struct cls_tarefa* proxima;
} cls_tarefa;

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


pool_de_threads cria_pool_de_threads(int numero_de_threads) {
    cls_threadpool *_pool;
    int i;

    if((numero_de_threads <= 0) || (numero_de_threads > MAX_THREADS))
        return NULL;

    _pool = (cls_threadpool *)malloc(sizeof(cls_threadpool));
    if(_pool == NULL){
        ferror("Erro na criação do pool");
        printf("\n");
        return NULL;
    }

    _pool->qtd_disponivel = numero_de_threads;
    _pool->tamanho_thread = 0;
    _pool->thread = NULL;
    _pool->prox_thread = NULL;
    _pool->threads = (pthread_t *)malloc(numero_de_threads * sizeof(pthread_t));
    pthread_mutex_init(&(_pool->bloqueador), NULL);
    pthread_cond_init(&(_pool->vazia), NULL);
    pthread_cond_init(&(_pool->nao_vazia), NULL);

    for(i = 0; i < _pool->qtd_disponivel; i++)
        if(pthread_create(&(_pool->threads[i]), NULL, trabalho_thread, _pool) != 0) {
            ferror("Erro criando thread");
            printf("\n");
            return NULL;
        }
    
    return (pool_de_threads)_pool;
}

void *trabalho_thread(pool_de_threads pool) {
    cls_threadpool *_pool = (cls_threadpool *)pool;
    cls_tarefa *tarefa_atual;

    pthread_mutex_lock(&(_pool->bloqueador));

    for(;;) {
        while(_pool->tamanho_thread == 0)
            pthread_cond_wait(&(_pool->nao_vazia), &(_pool->bloqueador));

        tarefa_atual = _pool->thread;
        _pool->tamanho_thread--;

        if(_pool->tamanho_thread == 0) {
            _pool->thread = NULL;
            _pool->prox_thread = NULL;
        }
        else
            _pool->thread = tarefa_atual->proxima;
        
        pthread_mutex_unlock(&(_pool->bloqueador));
        (tarefa_atual->rotina) (tarefa_atual->argumentos);

        free(tarefa_atual);        
    }

    pthread_mutex_unlock(&(_pool->bloqueador));
}

void enviar(pool_de_threads thread, envio funcao_de_envio, void *argumentos) {
    cls_threadpool *_pool = (cls_threadpool *)thread;
    cls_tarefa *tarefa;

    printf("passou aqui no enviar\n");
    tarefa = (cls_tarefa *)malloc(sizeof(cls_tarefa));
    if(tarefa == NULL) {
        ferror("Erro alocando espaço");
        return;
    }

    tarefa->rotina = funcao_de_envio;
    tarefa->argumentos = argumentos;
    tarefa->proxima = NULL;

    pthread_mutex_lock(&(_pool->bloqueador));
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
    printf("Ai fodeu\n");
}

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