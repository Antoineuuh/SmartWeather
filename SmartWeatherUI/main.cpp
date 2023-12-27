/*
* Name : Smart Weather UI ++
* Language : C++
* Author : Antoine MARCELLE
*/

#include "MainWindow.h"

#include <Includes.h>

#include <DbManager.h>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    app.setApplicationName("Smart Weather UI ++");
    app.setApplicationDisplayName("Smart Weather UI ++");
    app.setApplicationVersion("1.0");

    MainWindow w;
    w.setWindowIcon(QIcon(":/icons/img/icon.png"));
    w.show();

    return app.exec();
}
