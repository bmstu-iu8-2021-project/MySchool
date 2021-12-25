#include "./headers/loginwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow* lw = new LoginWindow();
    lw->show();
    return a.exec();
}
