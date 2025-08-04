#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "../backend/backend.h"

using namespace std;

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    while (true) {
        char* random_text = get_random_text();
        cout << "Random text: " << random_text << endl;

        this_thread::sleep_for(chrono::seconds(10));
    }

    return 0;
}
