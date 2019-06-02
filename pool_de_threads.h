/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Definição do nosso pool de threads
*/



// Constantes
#define NUMERO_MAXIMO_THREADS 100



// Estruturas
typedef void *pool_de_threads;
typedef void (*envio)(void *);



// Métodos
pool_de_threads cria_pool_de_threads(int numero_de_threads);
void *trabalho_thread(pool_de_threads pool);
void adiciona_tarefa_no_pool(pool_de_threads thread, envio funcao_de_envio, void *argumentos);
void encerra_pool(pool_de_threads pool_para_encerrar);