/*
* Name : Smart Weather UI ++
* Language : C++
* Author : Antoine MARCELLE
*/


// Importation des classes utiles au programme
#include "MainWindow.h"
#include <Includes.h>


/**
 * @brief @mainclass main permet la gestion de l'initialisation de l'application :
 * application, fenetre principale...
 */
int main(int argc, char *argv[]) {

    // Initialisationde l'application : nom, version
    QApplication app(argc, argv);
    app.setApplicationName("Smart Weather UI ++");
    app.setApplicationDisplayName("Smart Weather UI ++");
    app.setApplicationVersion("1.2");

    // Initialisation de la fenetre principale : aspects graphiques, icon, frameless
    MainWindow w;
    w.setWindowIcon(QIcon(":/icons/img/icon.png"));
    w.setWindowFlags(Qt::WindowType::FramelessWindowHint);
    w.show();

    // Execution de l'application
    return app.exec();
}
