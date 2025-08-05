#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#include "../backend/backend.h"

using namespace std;

int main() {

    backend_init();

    char* json = get_worldtime_json();
    
    if (json) {
        cout << "[FRONTEND] worldtimeapi JSON: " << json << endl;
        free(json);
    } else {
        cerr << "[FRONTEND] Falha ao requisitar worldtimeapi." << endl;
    }

    while (true) {
        char * random_text = get_random_text();
        cout << "Random text: " << random_text << endl;
        free(random_text);
        this_thread::sleep_for(chrono::seconds(10));
    }

    return 0;
}
