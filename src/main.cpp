#include <QApplication>
#include <QFile>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    // Load global stylesheet
    QFile qss(":/style.qss");
    if (qss.open(QFile::ReadOnly))
        app.setStyleSheet(qss.readAll());

    MainWindow w;
    w.resize(1100, 680);
    w.setWindowTitle("Serial Monitor");
    w.show();

    return app.exec();
}
