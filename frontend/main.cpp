#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../backend/backend.h"

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    char* random_text = get_random_text();
    std::cout << "Random text: " << random_text << std::endl;

    return 0;
}
