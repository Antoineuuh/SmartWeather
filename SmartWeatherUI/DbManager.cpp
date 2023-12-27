#include "DbManager.h"

#include <Includes.h>

DbManager::DbManager() {
    connexion = DbConnection("airports.sqlite3");
}


DbConnection DbManager::getDbConnection(){
    return connexion;
}

QString DbManager::lookForICAO(QString nameOfCity) {

    QSqlQuery* query = new QSqlQuery(getDbConnection().getConnection());

    if(nameOfCity.size() == 3 or 4){

        if(!query->exec("SELECT icao,iata,name,country,latitude,longitude FROM airports WHERE iata LIKE '" + nameOfCity + "' OR icao LIKE '" + nameOfCity + "' COLLATE NOCASE")) {

            qDebug() << "Erreur requete icao";
            return "";

        } else {

            if(!query->next()) {

                if(!query->exec("SELECT icao,iata,name,country,latitude,longitude FROM airports WHERE name LIKE '%" + nameOfCity + "%' OR city LIKE '%" + nameOfCity + "%' COLLATE NOCASE")) {

                    qDebug() << "Erreur requete icao";
                    return "";
                } else {

                    if(!query->next()) {
                        qDebug() << "Erreur aucun icao correspondant";
                        return "";
                    }

                }

            }
        }
    } else {

        if(!query->exec("SELECT icao,iata,name,country,latitude,longitude FROM airports WHERE name LIKE '%" + nameOfCity + "%' OR city LIKE '%" + nameOfCity + "%' COLLATE NOCASE")) {
            qDebug() << "Erreur requete icao";
            return "";
        }

        if(!query->next()) {
            qDebug() << "Erreur aucun icao correspondant";
            return "";
        }

    }

    return query->value(0).toString();

}

QStringList DbManager::findSimilarAirport(QString nameOfTheCity) {

    QSqlQuery* query = new QSqlQuery(getDbConnection().getConnection());

    if(!query->exec("SELECT airports.name FROM airports WHERE" + ((nameOfTheCity.size() == 3 || nameOfTheCity.size() == 4) ? (" iata LIKE '" + nameOfTheCity + "' OR icao LIKE '" + nameOfTheCity + "' OR ") : " ") + "airports.city LIKE '%" + nameOfTheCity + "%' OR airports.name LIKE '%" + nameOfTheCity + "%' COLLATE NOCASE")) {

        qDebug() << "Erreur de similitude - " << query->lastError() << " - " << query->executedQuery() ;
        return QStringList();

    }

    QStringList cityAirportList;

    while(query->next()) {
        cityAirportList << query->value(0).toString();
    }

    return cityAirportList;

}
