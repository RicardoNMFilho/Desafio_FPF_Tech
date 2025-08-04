#include <iostream>

extern "C" {
    #include "lib_gentexts.h"
}

using namespace std;



int main() {
    cout << "Backend running!" << endl;

    char** texts = generate_list_random_texts();

    for (int i = 0; i < 5; ++i) {
        std::cout << texts[i] << std::endl;
    }

    return 0;
}