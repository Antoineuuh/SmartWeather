#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <Includes.h>

/**
 * @brief @class MainWindow permet la gestion de la fenetre principale de l'application :
 * actualisation des données, evenements, mouvement de la fenetre...
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // Gestion de l'évenement de telechargement du fichier météo et de l'actions associée.
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNetworkInformation(QNetworkReply*)));
    networkManager->get(QNetworkRequest(QUrl("https://aviationweather-cprk.ncep.noaa.gov/adds/dataserver_current/current/metars.cache.csv")));

    // Actualisation des informations météo
    updateInformation();

    // Initialisation du lien clicable pour accéder au rapport météo complet
    ui->link_to_full_report->setText("<a style='font: 700 8pt Arial; color: rgb(176, 161, 220); text-decoration: none;' href='https://metar-taf.com/fr?hl=" + ICAO + "'>Afficher le rapport météo complet</a>");
    ui->link_to_full_report->setOpenExternalLinks(true);

}

// Deconstructeur, permet la fermeture de la fenetre
MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief getDbManager, permet de récupérer les méthodes du DbManager
 * @return DbManager, manager de la bdd
 */
DbManager MainWindow::getDbManager(){
    return *manager;
}

/**
 * @brief on_searchAirport_textChanged, evenement actionné lorsque le contenue de la barre de
 * recherche a été actualisée par l'utilisateur
 *
 * @param QString &newText, nouveau texte dans la barre de recherche
 */
void MainWindow::on_searchAirport_textChanged(const QString &newText) {

    // Si le texte dans la barre de recherche est de taille supérieure a 2
    if(newText.size() > 2) {

        // On récupere les aéroport similaires au param newText
        QStringList air = getDbManager().findSimilarAirport(newText);

        // On créé un completer/ liste deroulante avec les aeroport similaires puis on l'affiche
        completer = new QCompleter(air, this);
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        completer->setCaseSensitivity(Qt::CaseSensitive);
        completer->setMaxVisibleItems(10);
        ui->searchAirport->setCompleter(completer);

    } else {
        // Sinon, on créé un completer vide
        ui->searchAirport->setCompleter(new QCompleter(this));
    }

}

void MainWindow::on_searchAirport_textEdited(const QString &newText){

}
/**
 * @brief on_searchAirport_editingFinished, permet d'afficher les informations de l'aéroport lorsque l'utilisateur
 * a terminer de taperle nom de ce dernier dans la barre de recherche
 */
void MainWindow::on_searchAirport_editingFinished() {

    // On asigne notre variable globale au résultat de la recherche dans la base de donnée
    ICAO = getDbManager().lookForICAO(ui->searchAirport->text());

    // Actualisation des informations météo
    updateInformation();

}

/**
 * @brief on_buttonLicence_clicked, evenement lorsque l'utilisateur clique sur le bouton information (... en haut a droite de la fenetre)
 */
void MainWindow::on_buttonLicence_clicked() {

    QString txt;
    txt = "<h2>Smart Weather ++</h2>\n"
          "<h6>Version v1.0</h6>\n"
          "<p>Application open-source (voir <a href =\"https://github.com/Antoineuuh/SmartWeather\">projet GitHub</a>)</p>\n"
          "<p>Développé par <a href=\"https://github.com/antoineuuh\">@Antoineuuh</a></p>\n";

    // On affiche une fenetre d'information avec le contenu du texte ci dessus
    QMessageBox::information(this, "Info", txt);

}

/**
 * @brief on_buttonClose_clicked, evenement lorsque l'utilisateur clique sur le bouton fermeture (X en haut a gauche de la fenetre)
 */
void MainWindow::on_buttonClose_clicked() {
    // On ferme l'application avec aucun code erreur
    QApplication::quit();
}

/**
 * @brief updateInformation, méthode qui permet l'actualisation de tous les label... etc de l'application avec les données météos de l'aéroport recherché
 */
void MainWindow::updateInformation(){

    // Initialisation d'une nouvelle requete SQL
    QSqlQuery* query = new QSqlQuery(manager->getDbConnection().getConnection());

    // Requete SQL pour recupérer toutes les infromations correspondant a un aéroport dans la table "airports" et "metars" a partir de son code ICAO
    if(!query->exec("SELECT airports.icao, "
                     "airports.iata, "
                     "airports.latitude,"
                     "airports.longitude, "
                     "airports.name, "
                     "metars.timest, "
                     "airports.country, "
                     "metars.temperature, "
                     "metars.dewpoint, "
                     "metars.winddir, "
                     "metars.windspeed, "
                     "metars.visibility, "
                     "metars.raw,"
                     "metars.cover "

                     "FROM airports "
                     "LEFT JOIN metars "
                     "ON airports.icao = metars.icao "
                     "WHERE airports.icao = '" + ICAO + "'")) {

        // Si la requete n'aboutie a aucun résultat, on notifie l'utilisateur
        qDebug() << "Erreur ! Echec de la récupération des infos.";

        // On affiche "N/A" dans toutes les cases de l'application
        ui->nameAirport->setText("Station : N/A");
        ui->date->setText("");
        ui->temperature->setText("N/A");
        ui->drew_point->setText("N/A");
        ui->windDir->setText("N/A");
        ui->windSpeed->setText("N/A");
        ui->visibility->setText("N/A");
        ui->pressure->setText("N/A");
        ui->cloud->setText("N/A");

        return;

    }

    QString iata, name, country, temp, dewpoint, winddir, windspeed, visibility, pressure, cover;

    // Si la requete a trouvée, au moins, un résultat
    if(query->next()) {

        // On encode/convertie les données récupérées dans un format prédéfini
        iata = (query->value(1).toString() != "NULL" ? query->value(1).toString() : "Inconnu");
        name = query->value(4).toString();
        country = (query->value(6).toString() != "NULL" ? query->value(6).toString() : "Inconnu");
        temp = (query->value(7).toString() != "NULL" ? query->value(7).toString() + "°" : "N/A");
        dewpoint = (query->value(8).toString() != "NULL" ? "Point de rosée: " + query->value(8).toString() + "°" : "N/A");
        winddir = query->value(9).toString() + "°";
        windspeed = QString::number(round(query->value(10).toDouble()*1.852*10)/10) + " km/h";
        visibility = (query->value(11).toString() == "6.21") ? "> 10 km" : QString::number((round((query->value(11).toDouble())*1.609344*10))/10) + " km";
        pressure = query->value(12).toString() + " hPa";
        cover = query->value(13).toString();

    } else {

        // Si la requete n'a pas trouvée, au moins, un résultat, on notifie l'utilisateur
        qDebug() << "Erreur ! Echec de la récupération des infos.";

        // On affiche "N/A" dans toutes les cases de l'application
        ui->nameAirport->setText("Station : N/A");
        ui->date->setText("");
        ui->temperature->setText("N/A");
        ui->drew_point->setText("N/A");
        ui->windDir->setText("N/A");
        ui->windSpeed->setText("N/A");
        ui->visibility->setText("N/A");
        ui->pressure->setText("N/A");
        ui->cloud->setText("N/A");

        return;

    }

    // On affiche les données ainsi récupérées dans toutes les cases de l'application
    ui->nameAirport->setText(name + ", " + country);
    ui->date->setText(QDateTime::currentDateTime().toString("dd-MMM-yy"));
    ui->temperature->setText(temp);
    ui->drew_point->setText(dewpoint);
    ui->windDir->setText(winddir);
    ui->windSpeed->setText(windspeed);
    ui->visibility->setText(visibility);
    ui->pressure->setText(pressure);


    // Gestion de l'image du nuage
    // On commence par creer un dictionnaire qui au code, renvoie la définition
    std::map<QString, QString> skyCover;
    skyCover["SKC"] = "Sky Clear";
    skyCover["FEW"] = "Few Clouds";
    skyCover["SCT"] = "Scattered";
    skyCover["BKN"] = "Broken";
    skyCover["OVC"] = "Overcast";
    skyCover["NSC"] = "No Significant Cloud";
    skyCover["NCD"] = "No Cloud Detected";
    skyCover["CAVOK"] = "Cavok";
    skyCover["CLR"] = "Sky Clear";

    // Ensuite, on assignie chaque code, a une image
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

    // On cherche pour la couverture nuageuse donnée, sa définition et son image
    std::map<QString, QString>::iterator iteration = skyCover.find(cover);
    std::map<QString, QPixmap>::iterator iteration2 = skyImages.find(cover);

    // S'il la recherche dans les tableaux ont abouties
    if(iteration != skyCover.end() && iteration2 != skyImages.end()) {

        // Si c'est trouvé, on met la definition et l'image dans l'application
        ui->cloud->setText(iteration->second);
        ui->cloudImage->setPixmap(iteration2->second);

    } else {

        // Sinon, on affiche une image et une definition par default
        ui->cloud->setText(cover);
        ui->cloudImage->setPixmap(QPixmap(":/icons/img/clouds/skc.png", "PNG"));

    }

    // Actualisation du lien clicable pour accéder au rapport météo complet
    ui->link_to_full_report->setText("<a style='font: 700 8pt Arial; color: rgb(176, 161, 220); text-decoration: none;' href='https://metar-taf.com/fr?hl=" + ICAO + "'>Afficher le rapport météo complet</a>");
    ui->link_to_full_report->setOpenExternalLinks(true);

}


/**
 * @brief updateNetworkInformation, permet le traitement/séparation des informations du fichier météo
 * @param QNetworkReply *response, le fichier rapport météo téléchargé sur internet
 */
void MainWindow::updateNetworkInformation(QNetworkReply *response) {

    /**
     *  Lors du téléchargement du fichier sur internet, l'application se met en mode "ne répond plus".
     *  Pour pallier a ce probleme et crééer une UI interactive, on affiche le xaiting screen pour
     *  notifier a l'utilisateur que les données sont en train d'etre téléchargées.
     */

    waiting_screen.show();


    /**
     * Plusieur threads sont alors éxécutés en meme temps, le download du rapport météo et le waiting screen
     */
    QTimer::singleShot(0, this, [=]() {

        // Initialisation d'une nouvelle requete SQL
        QSqlQuery* query = new QSqlQuery(manager->getDbConnection().getConnection());
        bool ok;

        // Code de reponse au telechargement du fichier en ligne
        int code = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&ok);

        // Si le code est = 200, c.a.d que le telechargement s'est bien passé, on peux alors traiter le fichier
        if(code == 200) {

            // Lecture du contenu de la réponse HTTP
            QString buffer = QString(response->read(20971520));

            // Déclaration de structures de données pour stocker les informations météorologiques
            QVector<QVector<QString>> infos;
            QVector<QString> line;
            QString cell("");

            // On supprime les données existantes dans la table "metars" de la base de données
            if(!query->exec("DELETE FROM metars")) {
                // Si ca n'aboutie pas, on notifie l'utilisateur
                qDebug() << "Erreur ! Echec du troncatage.";
            }

            // Parcours du contenu du fichier CSV téléchargé. Boucle pour parcourir chaque caractère du buffer
            for(int i = 0; i != buffer.size(); i++) {

                // Si le caractère est un retour à la ligne
                if(buffer[i] == '\n' || buffer[i] == '\r') {

                    // Ajout de la cellule courante à la ligne et réinitialisation de la cellule
                    line.push_back((cell == "" ? "NULL" : cell));
                    infos.push_back(line);
                    cell.clear();
                    line.clear();

                // Sinon, si le caractère est une virgule
                } else if(buffer[i] == ',') {

                    // Ajout de la cellule courante à la ligne et réinitialisation de la cellule
                    line.push_back((cell == "" ? "NULL" : cell));
                    cell.clear();

                } else {
                    // Sinon, s'il s'agit d'un caractère normal, ajout du caractère à la cellule
                    cell += buffer[i];
                }
            }

            // Suppression des 6 premières lignes qui contiennent des informations inutiles
            infos.remove(0,6);

            // Boucle pour traiter chaque ligne d'informations météorologiques
            for(int i = 0; i != infos.size(); i++) {

                // Utilisation d'une expression régulière pour extraire le code ICAO (ex: "Q1234" -> "1234")
                QRegularExpression regex("\\bQ\\d{3,4}\\b");
                QRegularExpressionMatch match = regex.match(infos[i][0]);

                // Insertion des informations dans la base de données
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
                                 "'" + match.captured().remove(0, 1) + "', "
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

            // Si le fichier n'a pas pu etre téléchargé en ligne (pas de wifi, probleme réseau...), on notifie l'utilisateur
            qDebug() << "Erreur ! Erreur de lecture du fichier téléchargé.";

            // On créé une fenetre demandant a l'utilisateur s'il veut réessayer de télécarhger ou quitter l'application
            QMessageBox msgBox;
            msgBox.setText("L'application n'a pas pu télécharger le bilan méteo.");
            msgBox.setInformativeText("Voulez-vous réessayer de le télécharger ?");
            msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Retry);
            msgBox.setDefaultButton(QMessageBox::Retry);
            msgBox.setWindowIcon(QIcon(":/icons/img/icon.png"));
            int retry = msgBox.exec();

            // En fonction du bouton sur lequel l'utilisateur cliqu dans cette fenetre, on effectue l'une des actions suivante
            switch(retry) {

            case QMessageBox::Retry:
                // On recommence le téléchargement
                networkManager->get(QNetworkRequest(QUrl("https://aviationweather-cprk.ncep.noaa.gov/adds/dataserver_current/current/metars.cache.csv")));
                break;

            case QMessageBox::Cancel:
                // On quitte l'application
                QApplication::quit();
                break;

            default:
                // DAns le as général, on quitte l'application
                QApplication::quit();
                break;

            }

            return;

        }

        // A la fin du thread, le téléchargement des données est terminé, on peut enlever le aiting screen
        waiting_screen.finish(this);

    });
}


/**
 * Initialement, on a définie notre type de fenetre comme étant "FramelessWindowHint"
 * c.a.d, sans bord, boutons close/pleine ecran etc...
 *
 * On souhaite cependant pouvoir faire bouger la fenetre de l'application.
 *
 * Les deux fonctions ci-dessous permettent cela via un clic souris maintenu et un mouvement de souris
 *
 * @param e, l'evenement correspondan
 */

void MainWindow::mousePressEvent(QMouseEvent *e) {
    current_pos = e->globalPosition().toPoint();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {

    new_pos = QPoint(e->globalPosition().toPoint() - current_pos);
    current_pos = e->globalPosition().toPoint();

    // Mouvement de la fenetre dans sa nouvelle position calculée si dessus
    move(x() + new_pos.x(), y() + new_pos.y());

}

