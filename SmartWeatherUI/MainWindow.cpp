#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <Includes.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNetworkInformation(QNetworkReply*)));
    networkManager->get(QNetworkRequest(QUrl("https://aviationweather-cprk.ncep.noaa.gov/adds/dataserver_current/current/metars.cache.csv")));

}

MainWindow::~MainWindow() {
    delete ui;
}

DbManager MainWindow::getDbManager(){
    return *manager;
}

void MainWindow::on_searchAirport_textChanged(const QString &newText) {

    if(newText.size() > 2) {

        QStringList air = getDbManager().findSimilarAirport(newText);
        completer = new QCompleter(air, this);
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        completer->setCaseSensitivity(Qt::CaseSensitive);
        completer->setMaxVisibleItems(10);
        ui->searchAirport->setCompleter(completer);

    } else {

        ui->searchAirport->setCompleter(new QCompleter(this));
    }

}

void MainWindow::on_searchAirport_textEdited(const QString &newText){

}

void MainWindow::on_searchAirport_editingFinished() {

    ICAO = getDbManager().lookForICAO(ui->searchAirport->text());

    updateInformation();

}

void MainWindow::on_buttonLicence_clicked() {

    QString txt;
    txt = "<h2>Smart Weather ++</h2>\n"
          "<h6>Version v1.0</h6>\n"
          "<p>Application open-source (voir <a href =\"https://github.com/Antoineuuh\">projet GitHub</a>)</p>\n"
          "<p>Développé par <a href=\"https://github.com/antoineuuh\">@Antoineuuh</a></p>\n";
    QMessageBox::information(this, "Info", txt);

}

void MainWindow::on_buttonClose_clicked() {
    QApplication::quit();
}

void MainWindow::updateInformation(){

    QSqlQuery* query = new QSqlQuery(manager->getDbConnection().getConnection());

    if(!query->exec("SELECT airports.icao, "
                     "airports.iata, "
                     "airports.latitude,"
                     "airports.longitude, "
                     "airports.name, "
                     "metars.timest, "
                     "airports.country, "
                     "metars.temperature, "
                     "metars.winddir, "
                     "metars.windspeed, "
                     "metars.visibility, "
                     "metars.cover "

                     "FROM airports "
                     "LEFT JOIN metars "
                     "ON airports.icao = metars.icao "
                     "WHERE airports.icao = '" + ICAO + "'")) {

        qDebug() << "Erreur ! Echec de la récupération des infos.";

        ui->nameAirport->setText("Station : N/A");
        ui->date->setText("");
        ui->temperature->setText("N/A");
        ui->windDir->setText("N/A");
        ui->windSpeed->setText("N/A");
        ui->visibility->setText("N/A");
        ui->pressure->setText("N/A");
        ui->cloud->setText("N/A");

        return;

    }

    QString iata, name, country, temp, winddir, windspeed, visibility, cover;

    if(query->next()) {

        iata = (query->value(1).toString() != "NULL" ? query->value(1).toString() : "Inconnu");
        name = query->value(4).toString();
        country = (query->value(6).toString() != "NULL" ? query->value(6).toString() : "Inconnu");
        temp = (query->value(7).toString() != "NULL" ? query->value(7).toString() + "°" : "N/A");
        winddir = query->value(8).toString() + "°";
        windspeed = QString::number(round(query->value(9).toDouble()*1.852*10)/10) + " km/h";
        visibility = (query->value(10).toString() == "6.21") ? "> 10 km" : QString::number((round((query->value(10).toDouble())*1.609344*10))/10) + " km";
        cover = query->value(11).toString();

        for(int i = 0; i < 12; i++) {
            qDebug() << query->value(i).toString();
        }

    } else {

        qDebug() << "Erreur ! Echec de la récupération des infos.";

        ui->nameAirport->setText("Station : N/A");
        ui->date->setText("");
        ui->temperature->setText("N/A");
        ui->windDir->setText("N/A");
        ui->windSpeed->setText("N/A");
        ui->visibility->setText("N/A");
        ui->pressure->setText("N/A");
        ui->cloud->setText("N/A");

        return;

    }

    ui->nameAirport->setText(name + ", " + country);
    ui->date->setText(QDateTime::currentDateTime().toString("dd-MMM-yy"));
    ui->temperature->setText(temp);
    ui->windDir->setText(winddir);
    ui->windSpeed->setText(windspeed);
    ui->visibility->setText(visibility);

    // On créé un dictionnaire qui au code, renvoie la définition
    std::map<QString, QString> skyCover;
    skyCover["SKC"] = "SKC";
    skyCover["FEW"] = "FEW";
    skyCover["SCT"] = "SCT";
    skyCover["BKN"] = "BKN";
    skyCover["OVC"] = "OVC";
    skyCover["NSC"] = "NSC";
    skyCover["NCD"] = "NCD";
    skyCover["CAVOK"] = "CAVOK";
    skyCover["CLR"] = "CLR";

    std::map<QString, QPixmap> skyImages;
    skyImages["SKC"] = QPixmap(":/icons/img/clouds/skc.png", "PNG");
    skyImages["FEW"] = QPixmap(":/icons/img/clouds/few.png", "PNG");
    skyImages["SCT"] = QPixmap(":/icons/img/clouds/sct.png", "PNG");
    skyImages["BKN"] = QPixmap(":/icons/img/clouds/bkn.png", "PNG");
    skyImages["OVC"] = QPixmap(":/icons/img/clouds/ovc.png", "PNG");
    skyImages["NSC"] = QPixmap(":/icons/img/clouds/few.png", "PNG");
    skyImages["NCD"] = QPixmap(":/icons/img/clouds/skc.png", "PNG");
    skyImages["CAVOK"] = QPixmap(":/icons/img/clouds/skc.png", "PNG");
    skyImages["CLR"] = QPixmap(":/icons/img/clouds/skc.png", "PNG");

    // On cherche pour le code donné
    std::map<QString, QString>::iterator iteration = skyCover.find(cover);
    std::map<QString, QPixmap>::iterator iteration2 = skyImages.find(cover);

    if(iteration != skyCover.end() && iteration2 != skyImages.end()) {

        // Si c'est trouvé, on met la définition
        ui->cloud->setText(iteration->second);
        ui->cloudImage->setPixmap(iteration2->second);

    } else {

        ui->cloud->setText(cover);
        ui->cloudImage->setPixmap(QPixmap(":/icons/img/clouds/skc.png", "PNG"));

    }

}

void MainWindow::updateNetworkInformation(QNetworkReply *response) {


    QSqlQuery* query = new QSqlQuery(manager->getDbConnection().getConnection());
    bool ok;
    int code = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&ok);

    if(code == 200) {

        QString buffer = QString(response->read(20971520));
        QVector<QVector<QString>> infos;
        QVector<QString> line;
        QString cell("");

        if(!query->exec("DELETE FROM metars")) {
            qDebug() << "Erreur ! Echec du troncatage.";
        }

        for(int i = 0; i != buffer.size(); i++) {

            if(buffer[i] == '\n' || buffer[i] == '\r') {

                line.push_back((cell == "" ? "NULL" : cell));
                infos.push_back(line);
                cell.clear();
                line.clear();

            } else if(buffer[i] == ',') {

                line.push_back((cell == "" ? "NULL" : cell));
                cell.clear();

            } else {

                cell += buffer[i];
            }
        }

        infos.remove(0,6);

        for(int i = 0; i != infos.size(); i++) {

            if(!query->exec("INSERT INTO metars("
                             "icao, "
                             "raw, "
                             "timest, "
                             "temperature, "
                             "dewpoint, "
                             "winddir, "
                             "windspeed, "
                             "visibility, "
                             "cover) "
                             "VALUES ("
                             "'" + infos[i][1] + "', "
                                             "'" + infos[i][0] + "', "
                                             "'" + infos[i][2] + "', "
                             + infos[i][5] + ", "
                             + infos[i][6] + ", "
                             + infos[i][7] + ", "
                             + infos[i][8] + ", "
                             + infos[i][10] + ", "
                                              "'" + infos[i][22] + "')")) {


                qDebug() << "Erreur ! " << query->lastError() << " - " << query->lastQuery();

            }

        }

    } else {

        qDebug() << "Erreur ! Erreur de lecture du fichier téléchargé.";

        QMessageBox msgBox;
        msgBox.setText("L'application n'a pas pu télécharger le bilan méteo.");
        msgBox.setInformativeText("Voulez-vous réessayer de le télécharger ?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Retry);
        msgBox.setDefaultButton(QMessageBox::Retry);
        msgBox.setWindowIcon(QIcon(":/icons/img/icon.png"));
        int retry = msgBox.exec();

        switch(retry) {

        case QMessageBox::Retry:
            networkManager->get(QNetworkRequest(QUrl("https://aviationweather-cprk.ncep.noaa.gov/adds/dataserver_current/current/metars.cache.csv")));
            break;

        case QMessageBox::Cancel:
            QApplication::quit();
            break;

        default:
            QApplication::quit();
            break;

        }

        return;

    }

}

