#define MAX_THREADS 100

typedef void *pool_de_threads;
typedef void (*envio)(void *);

pool_de_threads cria_pool_de_threads(int numero_de_threads);
void enviar(pool_de_threads thread, envio funcao_de_envio, void *argumentos);
void encerra_pool(pool_de_threads pool_para_encerrar);
void *trabalho_thread(pool_de_threads pool);