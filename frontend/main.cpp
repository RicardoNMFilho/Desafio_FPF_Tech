#include <QApplication>
#include "gui.h"
#include "../backend/backend.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    backend_init();

    MainWindow window;
    window.resize(550, 350);
    window.show();
    
    return app.exec();
}
