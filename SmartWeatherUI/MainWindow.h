#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Includes.h>
#include <DbManager.h>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    DbManager getDbManager();

private slots:

    void on_searchAirport_textChanged(const QString &newText);

    void on_searchAirport_textEdited(const QString &newText);

    void on_searchAirport_editingFinished();

    void on_buttonLicence_clicked();

    void on_buttonClose_clicked();

    void updateInformation();

    void updateNetworkInformation(QNetworkReply *resp);

private:

    Ui::MainWindow *ui;

    QCompleter *completer;

    DbManager *manager = new DbManager();

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    QVector<QString> infos;

    QVector<QString> names;

    QString ICAO = "LFPG";
};

#endif // MAINWINDOW_H
