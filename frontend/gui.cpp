#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QGroupBox>
#include <QFont>
#include <QRegularExpression>

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

    char* json = get_worldtime_json();
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

    auto updateText = [textLabel]() {
        char* new_text = get_random_text();
        if (new_text) {
            textLabel->setText(new_text);
            free(new_text);
        }
    };

    updateText();

    QTimer* textTimer = new QTimer(this);
    connect(textTimer, &QTimer::timeout, updateText);
    textTimer->start(10000);

    QTimer* elapsedTimer = new QTimer(this);
    connect(elapsedTimer, &QTimer::timeout, [elapsedLabel]() {
        double elapsed = get_elapsed_seconds();
        elapsedLabel->setText(QString("Tempo decorrido: %1s").arg((int)elapsed));
    });
    elapsedTimer->start(1000);
}
