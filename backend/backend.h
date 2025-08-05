#ifndef BACKEND_H
#define BACKEND_H


/**
 * Inicializa o backend, configurando a semente do gerador de números aleatórios
 * e registrando o tempo de início da aplicação.
 */
void backend_init();

/**
 * Realiza uma requisição HTTP GET para obter o horário atual de Manaus em formato JSON.
 * Retorna uma string alocada dinamicamente (deve ser liberada pelo usuário).
 */
char* get_worldtime_json();

/**
 * Retorna uma cópia de um texto aleatório do conjunto gerado na primeira chamada.
 * O sorteio é feito localmente, sem nova chamada à biblioteca dinâmica.
 * Retorna uma string alocada dinamicamente (deve ser liberada pelo usuário).
 */
char* get_random_text();

/**
 * Retorna o tempo (em segundos) decorrido desde a inicialização do backend.
 */
double get_elapsed_seconds();

#endif