#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qSetMessagePattern("log[%{file} %{line} ] %{message}");
    MainWindow w;
    w.setWindowTitle("FFUserDemo");
    w.show();

    return a.exec();
}
