#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QGroupBox>
#include <QFont>
#include <QRegularExpression>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QPixmap>
#include <QtConcurrent>

#include "gui.h"
#include "../backend/backend.h"

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QFont titleFont("Arial", 12, QFont::Bold);
    QFont contentFont("Arial", 10);

    QGroupBox* jsonBox = new QGroupBox("Horário Atual (WorldTimeAPI)", this);
    QVBoxLayout* jsonLayout = new QVBoxLayout(jsonBox);

    QLabel* jsonLabel = new QLabel("Horário não disponível", this);
    jsonLabel->setWordWrap(true);
    jsonLabel->setFont(contentFont);
    jsonLayout->addWidget(jsonLabel);
    mainLayout->addWidget(jsonBox);

    QGroupBox* textBox = new QGroupBox("Texto Aleatório", this);
    QVBoxLayout* textLayout = new QVBoxLayout(textBox);

    QLabel* textLabel = new QLabel("Gerando texto...", this);
    textLabel->setWordWrap(true);
    textLabel->setFont(contentFont);
    textLayout->addWidget(textLabel);
    mainLayout->addWidget(textBox);

    // Bloco para imagem
    QGroupBox* imageBox = new QGroupBox("Imagem", this);

    QVBoxLayout* imageLayout = new QVBoxLayout(imageBox);

    QLabel* imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    imageLabel->setFixedHeight(120);
    imageLabel->setVisible(false);

    imageLayout->addWidget(imageLabel);
    mainLayout->addWidget(imageBox);

    QLabel* elapsedLabel = new QLabel("Tempo decorrido: 0s", this);
    elapsedLabel->setAlignment(Qt::AlignCenter);
    elapsedLabel->setFont(titleFont);
    mainLayout->addWidget(elapsedLabel);

    this->setStyleSheet(R"(
        QWidget {
            background-color: #f0f4f8;
        }
        QGroupBox {
            border: 1px solid #c0c0c0;
            border-radius: 8px;
            margin-top: 10px;
            background-color: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
            font-weight: bold;
            color: #2c3e50;
        }
        QLabel {
            color: #333;
        }
    )");

    // Inicialização assíncrona: só abrir interface após resposta da API
    this->hide();

    QtConcurrent::run([this, jsonLabel, elapsedLabel, textLabel, imageLabel]() {

        char* json = get_worldtime_json();

        QMetaObject::invokeMethod(this, [=]() {

            if (json) {
                QString jsonStr(json);
                jsonLabel->setText(jsonStr);

                QString timezone, datetime;
                QRegularExpression tzRegex(QStringLiteral("\"timezone\"\\s*:\\s*\"([^\"]+)\""));
                QRegularExpression dtRegex(QStringLiteral("\"datetime\"\\s*:\\s*\"([^\"]+)\""));
                QRegularExpressionMatch tzMatch = tzRegex.match(jsonStr);
                QRegularExpressionMatch dtMatch = dtRegex.match(jsonStr);

                if (tzMatch.hasMatch() && dtMatch.hasMatch()) {
                    timezone = tzMatch.captured(1);
                    datetime = dtMatch.captured(1);
                    QString title = timezone + ", " + datetime;
                    setWindowTitle(title);
                } else {
                    setWindowTitle("Desafio FPF Tech (dados inválidos)");
                }

                free(json);
            } else {
                jsonLabel->setText("Falha ao requisitar worldtimeapi.");
                setWindowTitle("Desafio FPF Tech (erro na API)");
            }

            this->show();

            // Inicializa o timer ANTES da primeira escolha de string aleatória
            backend_init();


            // Função para atualizar texto, som e imagem
            auto updateText = [textLabel, imageLabel, this]() {
                char* new_text = get_random_text();
                if (new_text) {
                    textLabel->setText(new_text);
                    free(new_text);
                }

                static QMediaPlayer* player = nullptr;
                if (!player) {
                    player = new QMediaPlayer(this);
                    player->setMedia(QUrl::fromLocalFile("frontend/assets/sound.wav"));
                }

                player->stop();
                player->play();
                player->setVolume(100);
                QTimer::singleShot(1000, this, [player]() { player->stop(); });

                QPixmap pixmap("frontend/assets/image.jpg");
                if (!pixmap.isNull()) {
                    int maxWidth = imageLabel->parentWidget()->width() - 40;
                    QPixmap scaledPixmap = pixmap.scaled(maxWidth, imageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    imageLabel->setPixmap(scaledPixmap);
                    imageLabel->setVisible(true);
                    QTimer::singleShot(1000, this, [imageLabel]() {
                        imageLabel->clear();
                        imageLabel->setVisible(false);
                    });
                }
            };

            // Atualiza texto na inicialização
            updateText();

            // Timer único para relógio e troca de texto sincronizada
            QTimer* elapsedTimer = new QTimer(this);
            int lastTextUpdate = -1;
            connect(elapsedTimer, &QTimer::timeout, [elapsedLabel, updateText, &lastTextUpdate]() mutable {
                double elapsed = get_elapsed_seconds();
                elapsedLabel->setText(QString("Tempo decorrido: %1s").arg((int)elapsed));
                int elapsedInt = static_cast<int>(elapsed);
                if (elapsedInt % 10 == 0 && elapsedInt != lastTextUpdate) {
                    updateText();
                    lastTextUpdate = elapsedInt;
                }
            });
            elapsedTimer->start(1000);

        });

    });
}


/*
- MainWindow : Classe principal da interface, herda de QWidget. Recebe 'parent' (ponteiro para widget pai, padrão do Qt). Implementa toda a lógica visual e de integração com backend.

- QVBoxLayout : Layout vertical para organizar widgets em coluna. Usado para o layout principal e para os blocos (GroupBox).

- QGroupBox : Widget de agrupamento com título. Usado para separar visualmente seções: JSON, texto, imagem.

- QLabel : Widget para exibir texto ou imagem. Usado para mostrar o JSON, texto aleatório, tempo decorrido e imagem.

- QFont : Define fonte usada nos títulos e conteúdos.

- QRegularExpression / QRegularExpressionMatch : Usados para extrair campos do JSON retornado pela API (timezone, datetime).

- QMediaPlayer : Reproduz áudio (wav) ao trocar texto. Instanciado uma vez, reutilizado a cada troca.

- QPixmap : Carrega e exibe imagem ao trocar texto. Escala imagem para caber no widget.

- QTimer : Timer para atualizar relógio e sincronizar troca de texto a cada 10 segundos. Executa função de atualização a cada segundo.

- QtConcurrent::run : Executa chamada à API e inicialização do backend em thread separada, evitando travar a interface.

- QMetaObject::invokeMethod : Garante que atualizações visuais ocorram na thread principal do Qt.

- backend_init() : Função do backend que inicializa o conjunto de textos aleatórios.

- get_worldtime_json() : Função do backend que retorna JSON da API WorldTime.

- get_random_text() : Função do backend que retorna um texto aleatório do conjunto gerado.

- get_elapsed_seconds() : Função do backend que retorna o tempo decorrido desde inicialização.

================================================================================
Fluxo geral:
1. Interface é criada, mas fica oculta até obter resposta da API.
2. Chama backend para obter JSON da WorldTimeAPI (thread separada).
3. Atualiza título e label com dados do JSON.
4. Inicializa backend (gera textos aleatórios).
5. Atualiza texto, som e imagem na interface.
6. Timer atualiza relógio e troca texto a cada 10 segundos, sincronizado com uptime.

================================================================================
Parâmetros principais:
- parent (QWidget*): Widget pai, padrão do Qt.
- textLabel, jsonLabel, imageLabel, elapsedLabel: Labels para exibir dados.
- updateText: Função lambda para atualizar texto, som e imagem.
- elapsedTimer: Timer para atualizar relógio e sincronizar troca de texto.
================================================================================
*/