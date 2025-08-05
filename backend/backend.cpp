#include <iostream>
#include <dlfcn.h>
#include <cstring>
#include <ctime>
#include <curl/curl.h>

using namespace std;
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    strncat((char*)userp, (char*)contents, total);
    return total;
}

char* get_worldtime_json() {
    CURL* curl = curl_easy_init();
    if (!curl) return nullptr;

    static char buffer[8192] = {0};
    curl_easy_setopt(curl, CURLOPT_URL, "http://worldtimeapi.org/api/timezone/America/Manaus");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "[CURL ERROR] " << curl_easy_strerror(res) << " (code: " << res << ")" << endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return nullptr;
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return strdup(buffer);
}

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

