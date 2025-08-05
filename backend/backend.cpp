#include <iostream>
#include <dlfcn.h>
#include <cstring>
#include <ctime>
#include <curl/curl.h>

using namespace std;

static time_t start_time = 0;

/**
 * Função de callback utilizada pela libcurl para escrever dados recebidos de uma requisição HTTP.
 *
 * Parâmetros:
 * - contents: ponteiro para os dados recebidos
 * - size: tamanho de cada bloco de dados
 * - nmemb: número de blocos
 * - userp: ponteiro para o buffer de destino (passado via CURLOPT_WRITEDATA)
 *
 * Retorno:
 * - Retorna o total de bytes processados (size * nmemb), como esperado pela libcurl.
 *
 * Observação: Aqui, os dados recebidos são concatenados ao buffer de destino usando strncat.
 * O buffer deve ser grande o suficiente para armazenar toda a resposta.
 */
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    strncat((char*)userp, (char*)contents, total);
    return total;
}


/**
 * Realiza uma requisição HTTP GET para obter o horário atual de Manaus em formato JSON.
 * Utiliza a biblioteca libcurl para realizar a comunicação.
 *
 * Passos:
 * 1. Inicializa o objeto CURL com curl_easy_init().
 * 2. Define a URL de destino usando CURLOPT_URL.
 * 3. Define a função de callback para escrita dos dados recebidos (CURLOPT_WRITEFUNCTION).
 * 4. Define o buffer de destino para os dados recebidos (CURLOPT_WRITEDATA).
 * 5. Adiciona cabeçalhos HTTP (Accept e User-Agent) usando curl_slist_append e CURLOPT_HTTPHEADER.
 * 6. Executa a requisição com curl_easy_perform().
 * 7. Em caso de erro, exibe mensagem e retorna nullptr.
 * 8. Libera recursos e retorna uma cópia da resposta recebida.
 *
 * Retorno:
 * - Ponteiro para string alocada dinamicamente contendo o JSON recebido (deve ser liberado pelo usuário)
 * - nullptr em caso de erro
 */
char * get_worldtime_json() {
    CURL* curl = curl_easy_init();
    if (!curl) return nullptr;

    // Buffer estático para armazenar a resposta JSON
    static char buffer[8192] = {0};
    curl_easy_setopt(curl, CURLOPT_URL, "http://worldtimeapi.org/api/timezone/America/Manaus");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    // Adiciona cabeçalhos HTTP
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Executa a requisição HTTP
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


/**
 * Inicializa o backend, configurando a semente do gerador de números aleatórios
 * e registrando o tempo de início da aplicação.
 *
 * Observação: A função srand é importante para garantir aleatoriedade nas funções que usam rand().
 */
void backend_init() {
    srand(static_cast<unsigned int>(time(nullptr)));
    start_time = time(nullptr);
}


/**
 * Retorna o tempo (em segundos) decorrido desde a inicialização do backend.
 * Utiliza a função difftime para calcular a diferença entre o tempo atual e o tempo de início.
 *
 * @return Tempo decorrido em segundos
 */
double get_elapsed_seconds() {
    return difftime(time(nullptr), start_time);
}


/**
 * Gera um número aleatório entre min e max (inclusive).
 * Utiliza a função rand() da biblioteca padrão.
 *
 * @param min Valor mínimo
 * @param max Valor máximo
 * @return Número aleatório entre min e max
 */
int random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}


/**
 * Carrega dinamicamente a biblioteca compartilhada libgentexts.so usando dlopen,
 * obtém ponteiros para as funções de geração e liberação de textos via dlsym,
 * gera uma lista de textos aleatórios e retorna uma cópia de um deles.
 *
 * Explicação sobre dlfcn:
 * - dlopen: Carrega uma biblioteca compartilhada em tempo de execução. Recebe o caminho do arquivo e flags (RTLD_LAZY para resolução preguiçosa de símbolos).
 * - dlsym: Obtém o endereço de uma função ou variável exportada pela biblioteca. Recebe o handle retornado por dlopen e o nome do símbolo.
 * - dlclose: Libera a biblioteca carregada.
 * - dlerror: Retorna uma string descrevendo o último erro ocorrido nas operações acima.
 *
 * Explicação sobre as funções da libgentexts:
 * - generate_list_random_texts: Recebe ponteiro para int, retorna ponteiro para array de strings (char**), cada string é um texto gerado.
 * - free_list_random_texts: Recebe o array de textos e o número de textos, libera toda a memória alocada.
 *
 * Passos:
 * 1. Carrega a biblioteca libgentexts.so.
 * 2. Obtém ponteiros para as funções de geração e liberação de textos.
 * 3. Gera uma lista de textos aleatórios.
 * 4. Seleciona aleatoriamente um dos textos gerados.
 * 5. Faz uma cópia do texto selecionado (strdup).
 * 6. Libera a lista de textos e a biblioteca.
 * 7. Retorna o texto selecionado.
 *
 * Retorno:
 * - Ponteiro para string alocada dinamicamente contendo o texto selecionado (deve ser liberado pelo usuário)
 * - nullptr em caso de erro
 */
char* get_random_text() {
    void* handle = dlopen("../lib/libgentexts.so", RTLD_LAZY);

    if (!handle) {
        cerr << "Erro ao carregar libgentexts.so: " << dlerror() << endl;
        return nullptr;
    }

    dlerror();

    // Define tipos para ponteiros de função
    using generate_func = char** (*)(int*);
    using free_func = void (*)(char**, int);

    // Obtém ponteiros para as funções da biblioteca
    generate_func generate = (generate_func)dlsym(handle, "generate_list_random_texts");
    free_func release = (free_func)dlsym(handle, "free_list_random_texts");

    const char * error = dlerror();

    if (error) {
        cerr << "Erro ao localizar função: " << error << endl;
        dlclose(handle);
        return nullptr;
    }

    int count = 0;

    char ** texts = generate(&count);

    if (!texts || count == 0) {
        dlclose(handle);
        return nullptr;
    }

    int index = random_number(0, count - 1);
    char * selected = strdup(texts[index]);

    release(texts, count);
    dlclose(handle);
    return selected;
}

