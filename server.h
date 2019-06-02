/*
Exercício de Programação de Desenvolvimento de Sistemas Distribuídos
Professora Doutora Gisele da Silva Craveiro

Autores:
Lucas Pipa Cervera                  8094403
Paulo Henrique Freitas Guimarães    9390361

Definição do nosso servidor web
*/



// Métodos
void envia_resposta(int _socket, char *resposta, int tamanho_resposta);
char *processa_requisicao(char *requisicao);
void acao_da_thread(void *argumentos);