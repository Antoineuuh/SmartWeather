#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Includes.h>
#include <DbManager.h>

QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow;}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    // Constructeur
    MainWindow(QWidget *parent = nullptr);

    // Destructeur
    ~MainWindow();

    // Getter
    DbManager getDbManager();

private slots:

    // Définition des evenements/callback

    void on_searchAirport_textChanged(const QString &newText);

    void on_searchAirport_textEdited(const QString &newText);

    void on_searchAirport_editingFinished();

    void on_buttonLicence_clicked();

    void on_buttonClose_clicked();

    void updateInformation();

    void updateNetworkInformation(QNetworkReply *resp);

private:

    Ui::MainWindow *ui;

    QCompleter *completer; // Liste déroulante des suggestions des choix d'aéroports

    DbManager *manager = new DbManager();

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this); // Acces internet pour téléchargelent des fichiers météos en ligne

    QVector<QString> infos;

    QVector<QString> names;

    QString ICAO = "LFPG";


    // Gestion du waiting screen lors du téléchargelent des données
    QPixmap waiting_screen_image = QPixmap(":/icons/img/splash.png");
    QSplashScreen waiting_screen = QSplashScreen(waiting_screen_image);

    // Gestion du déplacement de la fenetre via la souris
    QPoint current_pos;
    QPoint new_pos;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // MAINWINDOW_H
