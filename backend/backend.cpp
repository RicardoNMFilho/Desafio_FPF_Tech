#include <iostream>
#include <cstdlib>
#include <ctime>

extern "C" {
    #include "lib_gentexts.h"
}

using namespace std;

int random_number(int min, int max) {
    int number;

    number = min + rand() % (max - min + 1);

    return number;
}

char * get_random_text () {
    char ** texts = generate_list_random_texts();

    int number = random_number(0, 4);

    return texts[number];
}