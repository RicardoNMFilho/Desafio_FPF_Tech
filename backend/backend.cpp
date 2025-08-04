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


int main() {
    cout << "Backend running!" << endl;

    srand(static_cast<unsigned int>(time(nullptr)));

    char * random_text = get_random_text();
    cout << random_text << endl;

    return 0;
}