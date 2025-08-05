
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lib_gentexts.h"

// Macros que definem limites para geração de textos e strings
// MAX_STRINGS e MIN_STRINGS: quantidade máxima e mínima de strings em um texto
#define MAX_STRINGS 20
#define MIN_STRINGS 5

// MAX_CHARS e MIN_CHARS: quantidade máxima e mínima de caracteres em cada string
#define MAX_CHARS 10
#define MIN_CHARS 3

// MAX_TEXTS e MIN_TEXTS: quantidade máxima e mínima de textos gerados em uma lista
#define MAX_TEXTS 8
#define MIN_TEXTS 5


/**
 * Gera um número aleatório entre min e max (inclusive).
 * Utiliza a função rand() da biblioteca padrão, que deve ser inicializada com srand() externamente.
 *
 * @param min Valor mínimo
 * @param max Valor máximo
 * @return Número aleatório entre min e max
 */
int random_number(int min, int max) {
    int number;
    number = min + rand() % (max - min + 1);
    return number;
}


/**
 * Gera uma string aleatória composta apenas por letras minúsculas.
 * O tamanho da string é definido aleatoriamente entre MIN_CHARS e MAX_CHARS.
 *
 * @return Ponteiro para a string alocada dinamicamente (deve ser liberada pelo usuário)
 *         ou NULL em caso de erro de alocação.
 */
char * generate_string() {
    int len = random_number(MIN_CHARS, MAX_CHARS);
    char *str = (char *)malloc((len + 1) * sizeof(char));

    if (str == NULL) {
        return NULL; 
    }

    // Preenche a string com caracteres aleatórios de 'a' a 'z'
    for (int i = 0; i < len; i++) {
        str[i] = 'a' + random_number(0, 25); 
    }

    str[len] = '\0'; // Finaliza a string

    return str;
}


/**
 * Gera um texto composto por várias strings aleatórias separadas por espaço.
 * O número de strings é definido aleatoriamente entre MIN_STRINGS e MAX_STRINGS.
 *
 * @return Ponteiro para o texto alocado dinamicamente (deve ser liberado pelo usuário)
 *         ou NULL em caso de erro de alocação.
 *
 * Observação: O texto é construído incrementalmente usando realloc, o que pode ser ineficiente
 * para textos grandes, mas é suficiente para o propósito deste gerador simples.
 */
char * generate_text() {
    int num_strings = random_number(MIN_STRINGS, MAX_STRINGS);
    char * text = (char *) malloc(1);
    char * str;

    text[0] = '\0';

    for (int i = 0; i < num_strings; i++) {
        str = generate_string();

        if (str == NULL) {
            free(text); 
            return NULL;
        }

        // Realoca o texto para acomodar a nova string e o espaço
        text = (char *)realloc(text, strlen(text) + strlen(str) + 2);

        if (text == NULL) {
            free(str);
            return NULL;
        }

        strcat(text, str);
        strcat(text, " "); // Adiciona espaço após cada string
        free(str);
    }

    return text;
}



/**
 * Gera uma lista de textos aleatórios.
 * O número de textos é definido aleatoriamente entre MIN_TEXTS e MAX_TEXTS.
 * Caso o ponteiro num_texts seja fornecido, o valor gerado é armazenado nele.
 *
 * @param num_texts Ponteiro para armazenar o número de textos gerados (opcional)
 * @return Ponteiro para o array de textos (cada texto é uma string alocada dinamicamente)
 *         ou NULL em caso de erro de alocação.
 *
 * Observação: Em caso de erro na geração de algum texto, todos os textos já gerados são liberados.
 */
char ** generate_list_random_texts(int *num_texts) {
    int n = random_number(MIN_TEXTS, MAX_TEXTS);
    if (num_texts) *num_texts = n;
    char ** list = (char **) malloc(n * sizeof(char *));
    if (list == NULL) {
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        list[i] = generate_text();
        if (list[i] == NULL) {
            // Libera todos os textos já gerados em caso de erro
            for (int j = 0; j < i; j++) {
                free(list[j]);
            }
            free(list);
            return NULL;
        }
    }
    return list;
}


