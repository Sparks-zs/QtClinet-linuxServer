#include "./mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = MainWindow::getInstance();
    w->show();
    return a.exec();
}
