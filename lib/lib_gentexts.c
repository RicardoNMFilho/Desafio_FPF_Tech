#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lib_gentexts.h"

#define MAX_STRINGS 20
#define MIN_STRINGS 5

#define MAX_CHARS 10
#define MIN_CHARS 3

#define MAX_TEXTS 8
#define MIN_TEXTS 5

int random_number(int min, int max) {
    int number;

    number = min + rand() % (max - min + 1);

    return number;
}

char * generate_string() {
    int len = random_number(MIN_CHARS, MAX_CHARS);
    char *str = (char *)malloc((len + 1) * sizeof(char));

    if (str == NULL) {
        return NULL; 
    }

    for (int i = 0; i < len; i++) {
        str[i] = 'a' + random_number(0, 25); 
    }

    str[len] = '\0'; 

    return str;
}

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

        text = (char *)realloc(text, strlen(text) + strlen(str) + 2);

        if (text == NULL) {
            free(str);

            return NULL;
        }

        strcat(text, str);
        strcat(text, " "); 
        free(str);
    }

    return text;
}


char ** generate_list_random_texts() {
    int num_texts = random_number(MIN_TEXTS, MAX_TEXTS);

    char ** list = (char **) malloc(num_texts * sizeof(char *)); 

    if (list == NULL) {

        return NULL; 

    }

    for (int i = 0; i < num_texts; i++) {
        list[i] = generate_text();

        if (list[i] == NULL) {

            for (int j = 0; j < i; j++) {
                free(list[j]);
            }

            free(list);

            return NULL;
        }
    }

    return list;
}