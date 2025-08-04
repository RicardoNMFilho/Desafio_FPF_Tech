#include <iostream>
#include <dlfcn.h>
#include <cstring>
#include <ctime>

using namespace std;

void backend_init() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

int random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}

char* get_random_text() {
    void* handle = dlopen("../lib/libgentexts.so", RTLD_LAZY);
    if (!handle) {
        cerr << "Erro ao carregar libgentexts.so: " << dlerror() << endl;
        return nullptr;
    }

    dlerror();

    using generate_func = char** (*)(int*);
    using free_func = void (*)(char**, int);

    generate_func generate = (generate_func)dlsym(handle, "generate_list_random_texts");
    free_func release = (free_func)dlsym(handle, "free_list_random_texts");

    const char* error = dlerror();
    if (error) {
        cerr << "Erro ao localizar função: " << error << endl;
        dlclose(handle);
        return nullptr;
    }

    int count = 0;
    char** texts = generate(&count);
    if (!texts || count == 0) {
        dlclose(handle);
        return nullptr;
    }

    int index = random_number(0, count - 1);
    char* selected = strdup(texts[index]);

    release(texts, count);
    dlclose(handle);
    return selected;
}
