#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <DbConnection.h>
#include <Includes.h>

class DbManager {

public:

    // Constructeurs
    DbManager();

    // Getter
    DbConnection getDbConnection();

    // Méthodes spécifiques
    QString lookForICAO(QString nameOfCity);
    QStringList findSimilarAirport(QString nameOfTheCity);


private:

    DbConnection connexion;
    QSqlQuery *query; // Requete sql

};

#endif // DBMANAGER_H
