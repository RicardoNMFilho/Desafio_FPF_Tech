# Projeto C++: Integração Modular com Qt, Docker e Biblioteca Dinâmica

## Descrição do Projeto

Este projeto foi desenvolvido como parte de uma avaliação técnica de C++. O objetivo era implementar uma aplicação gráfica modular, com integração entre código C e C++, recursos multimídia, consumo de API externa e execução em ambiente Linux via Docker.

A aplicação exibe um texto aleatório (fornecido por uma biblioteca dinâmica em C) a cada 10 segundos, juntamente com uma imagem e som. O título da janela apresenta informações obtidas da [API pública](http://worldtimeapi.org/api/timezone/America/Manaus), e o tempo de execução da aplicação é exibido em tempo real.

## Escolha de Tecnologias

- **Docker**  
  Utilizado para garantir a execução em ambiente Ubuntu, conforme exigido, mesmo em uma máquina com Windows 11.

- **CMake**  
  Ferramenta de build modular conforme solicitado no enunciado.

- **`dlfcn`**  
  Biblioteca POSIX usada para carregamento dinâmico de bibliotecas. Permitindo separar o backend da lógica de geração de textos.

- **libcurl**  
  Utilizada para realizar requisições HTTP em C++. Foi a primeira vez que integrei esse tipo de funcionalidade com C++, o que exigiu leitura de documentação específica.

- **Qt (via C++)**  
  Usado no frontend. A experiência prévia com PyQt (Python) facilitou a familiarização com sua versão em C++.

- **Xming e XLaunch**  
  Utilizados para redirecionar a interface gráfica do container Docker para o host Windows via protocolo X11.

## Arquitetura do Projeto

### 1. Biblioteca de Geração de Textos (`lib_gentexts.c`, `lib_gentexts.h`)

- Escrita em C puro.
- Expõe a função `generate_list_random_texts()`, que retorna um array de strings aleatórias.
- A biblioteca é carregada dinamicamente pelo backend e mantém os textos em memória.

### 2. Backend (`backend.cpp`, `backend.h`)

- Escrita em C++.
- Responsável por:
  - Carregamento dinâmico da biblioteca com `dlopen`
  - Armazenamento em cache dos textos gerados
  - Sorteio aleatório dos textos
  - Chamada da API externa com `libcurl`
  - Medição do tempo de execução desde o início
- Expõe funções como `get_random_text()`, `backend_init()`, `get_worldtime_json()`, entre outras.

### 3. Frontend (`gui.cpp`)

- Desenvolvido com Qt5 em C++.
- Interface gráfica principal da aplicação.
- Atualiza o texto exibido a cada 10 segundos, reproduz um som e exibe uma imagem.
- Mostra no título da janela os dados da API (`timezone, datetime`).
- Atualiza o tempo de execução na tela a cada segundo.

## Principais Desafios

- **Ambiente de Desenvolvimento**  
  Como o desenvolvimento foi feito em um sistema Windows 11, foi necessário utilizar Docker desde o início para simular corretamente o ambiente Linux (Ubuntu), evitando surpresas na execução.

- **Uso de `dlfcn` e Carga Dinâmica**  
  Esta foi a primeira vez que trabalhei com a API `dlfcn`. Foi necessário estudar sua documentação para entender a correta manipulação de `dlopen`, `dlsym` e `dlclose`.

- **Transição de Linkagem Estática para Dinâmica**  
  Durante os testes iniciais, a biblioteca foi integrada de forma estática para validar o fluxo mínimo da aplicação. Posteriormente, o desafio foi ajustar o projeto para suportar carregamento dinâmico, exigindo alterações no `CMakeLists.txt` e na estrutura do projeto.

- **Execução de Interface Gráfica e Som em Containers**  
  Como containers não possuem, por padrão, suporte a exibição gráfica ou áudio, foi necessário configurar o redirecionamento da interface com Xming. O som, no entanto, permaneceu indisponível. Isso gerou erros como:

  ```
  GStreamer; Unable to pause - "file:frontend/assets/sound.wav"
  GStreamer; Unable to play - "file:frontend/assets/sound.wav"
  Warning: "Failed to connect: Connection refused"
  Error: "Could not open resource for reading."
  ```

- **Estruturação do Build com CMake**  
  A tentativa inicial de dividir o projeto em múltiplos arquivos `CMakeLists.txt` (um por módulo) gerou complexidade desnecessária. Ao final, optei por centralizar tudo em um único `CMakeLists.txt` no diretório raiz.

- **Primeira Experiência Real com Interface Gráfica em C++**  
  Apesar da familiaridade com desenvolvimento de interfaces usando PyQt, foi a primeira vez que trabalhei com um projeto gráfico em C++ de forma completa, com atualizações em tempo real, integração multimídia e manipulação de estado.

## Instruções de Execução

Este projeto foi desenvolvido para ser executado em ambiente Linux utilizando Docker, com interface gráfica Qt. As instruções abaixo consideram o uso do Windows como sistema host.

### Pré-requisitos

- Docker instalado  
- Servidor X11 rodando no Windows (ex: Xming ou VcXsrv)  
- Variável de ambiente `DISPLAY` configurada corretamente no Windows  

### Passos para Execução

1. **Build da imagem Docker**  
   No diretório raiz do projeto, execute:

   ```sh
   docker build -t desafioFPF .
   ```

2. **Execução do container**  
   No Windows PowerShell, execute:

   ```sh
   docker run --rm -e DISPLAY=$env:DISPLAY desafioFPF
   ```

   > Certifique-se de que o servidor X11 está rodando e que o Docker está autorizado a acessar o display.

3. **O que esperar**
   - A interface gráfica Qt será exibida na tela do Windows.
   - O frontend mostrará o horário atual de Manaus (obtido via API), um texto aleatório, imagem e som.
   - O texto aleatório será sorteado a cada 10 segundos, sincronizado com o tempo decorrido.

### Observações

- Caso não veja a interface gráfica, verifique se o servidor X11 está ativo e se a variável `DISPLAY` está correta.
- O backend consome a API [WorldTimeAPI](http://worldtimeapi.org/) para exibir o horário.
- O conjunto de textos aleatórios é gerado uma única vez, e os sorteios subsequentes ocorrem sobre o conteúdo disponível no cache.

### Finalização

- Ao fechar a interface, todos os recursos alocados (textos, biblioteca dinâmica) são liberados automaticamente.

## Como executar no Linux

Para rodar a aplicação no Linux usando Docker com suporte à interface gráfica Qt e saída de áudio via PulseAudio, siga os passos abaixo.

### Pré-requisitos

- Docker instalado e funcionando no seu Linux.
- Servidor X11 ativo (normalmente já vem configurado no Linux).
- PulseAudio rodando no host Linux.

### Comandos

No terminal, defina a variável do servidor PulseAudio:

```bash
export PULSE_SERVER=unix:${XDG_RUNTIME_DIR}/pulse/native
```

Depois execute o container Docker com:

```bash
sudo docker run -it \
  -e DISPLAY=$DISPLAY \
  -e PULSE_SERVER=$PULSE_SERVER \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native \
  -v ~/.config/pulse/cookie:/root/.config/pulse/cookie \
  -v $(pwd)/frontend/assets:/app/frontend/assets \
  desafio-fpf-tech
```

### Explicações

- `-e DISPLAY=$DISPLAY` : Permite acessar o servidor X11 para exibir a interface gráfica.  
- `-e PULSE_SERVER=$PULSE_SERVER` : Permite acesso ao servidor de áudio PulseAudio do host.  
- `-v /tmp/.X11-unix:/tmp/.X11-unix` : Monta o socket do X11 para comunicação gráfica.  
- `-v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native` e `-v ~/.config/pulse/cookie:/root/.config/pulse/cookie` : Permitem que o container use o áudio via PulseAudio do host.  
- `-v $(pwd)/frontend/assets:/app/frontend/assets` : Monta a pasta local com arquivos multimídia para o container.

### Notas

- Certifique-se que o arquivo `sound.wav` está dentro da pasta `frontend/assets` na raiz do projeto.  
- A variável `XDG_RUNTIME_DIR` geralmente é algo como `/run/user/1000`. Você pode verificar com `echo $XDG_RUNTIME_DIR`.  
- Execute os comandos no diretório raiz do projeto (onde está a pasta `frontend`).

## Documentações e Tutoriais Utilizados

| Link | Descrição |
|------|-----------|
| [TLDP - Program Library HOWTO](https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html) | Guia prático para bibliotecas dinâmicas |
| [POSIX `dlfcn.h` Reference](https://pubs.opengroup.org/onlinepubs/7908799/xsh/dlfcn.h.html) | Documentação técnica da API `dlfcn` |
| [Terminal Root - curl com C++](https://terminalroot.com.br/2021/05/utilizando-curl-com-cpp.html) | Tutorial introdutório para uso de `libcurl` |
| [Libcurl Documentation](https://curl.se/libcurl/) | Documentação oficial da biblioteca |
| [Zetcode Qt5 C++ Tutorial](https://zetcode.com/gui/qt5/) | Guia para desenvolvimento com Qt5 |

## Bibliotecas Instaladas no Dockerfile

Abaixo estão as principais bibliotecas e ferramentas instaladas via Dockerfile, com suas justificativas:

- **build-essential, g++**: Ferramentas básicas para compilação de código C/C++.
- **cmake**: Sistema de build utilizado para organizar e compilar os módulos do projeto.
- **libcurl4-openssl-dev**: Biblioteca para consumo de APIs HTTP, utilizada no backend para acessar a WorldTimeAPI.
- **ca-certificates**: Certificados de CA necessários para conexões HTTPS seguras.
- **qtbase5-dev, qt5-qmake, qtbase5-dev-tools, qtchooser**: Componentes essenciais para desenvolvimento e execução de aplicações Qt.
- **qtmultimedia5-dev, qt5-image-formats-plugins, libqt5concurrent5, libqt5multimedia5-plugins**: Suporte a multimídia (áudio, imagem) e concorrência, necessários para reprodução de som, exibição de imagens e uso de timers no frontend.
- **gstreamer1.0-plugins-base, gstreamer1.0-plugins-good, gstreamer1.0-alsa**: Plugins necessários para reprodução de áudio/vídeo no QtMultimedia.
- **libx11-xcb1, libxcb1, libxcb-util1, libxrender1, libxi6, libxext6, libxfixes3, libxrandr2, libxinerama1, libxcursor1, libxkbcommon0, libxkbcommon-x11-0, libgl1-mesa-glx, libglib2.0-0**: Bibliotecas gráficas necessárias para execução de aplicações Qt com interface gráfica em ambiente Linux.

## Considerações Finais e Pontos de Melhoria

- **Testes Unitários e Integrados**: O projeto não possui testes automatizados. O ideal seria implementar testes unitários para as funções principais (especialmente da biblioteca de textos e backend) e testes integrados para garantir o funcionamento do sistema como um todo. Ferramentas como CTest, Google Test ou Catch2 poderiam ser utilizadas.

- **Comentários em Português**: Todos os comentários do código estão em português.

- **Versionamento**: O versionamento do projeto poderia seguir um padrão formal, como [Semantic Versioning](https://semver.org/), e utilizar tags e mensagens de commit mais detalhadas para facilitar o acompanhamento da evolução do código.

- **Ambiente de Desenvolvimento**: O ideal teria sido utilizar uma máquina Linux como host para evitar as complexidades de redirecionamento gráfico e possíveis limitações de áudio. O uso do Docker permitiu contornar essa limitação, mas pode impactar a experiência de uso e depuração.
