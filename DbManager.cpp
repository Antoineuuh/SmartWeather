#include "DbManager.h"
#include <Includes.h>

/**
 * @brief @class DbManager permet la gestion des requetes sql
 * recherche d'aéroports, icao...
 */
DbManager::DbManager() {
    connexion = DbConnection("airports.sqlite3");
}

/**
 * @brief @name getDbConnection permet d'avoir acces a la connexion de la bdd
 *
 * @param none
 *
 * @return DbConnection database, renvoie une connexion a la bdd
 */
DbConnection DbManager::getDbConnection(){
    return connexion;
}

/**
 * @brief @name lookForICAO permet de récuperer l'ICAO d'un aeroport par rapport a son nom, sa ville, son code iata
 *
 * @param QString nameOfCity, nom de la ville ou chercher un aéroport
 *
 * @return QString, ICAO correspondant
 */
QString DbManager::lookForICAO(QString nameOfCity) {

    // Initialisation d'une nouvelle requete SQL
    QSqlQuery* query = new QSqlQuery(getDbConnection().getConnection());

    // Si le parametre nameOfCity a la taille d'un code ICAO (cdg = lfpg, bdx = lfbd...)
    if(nameOfCity.size() == 3 or 4){

        // Requete SQL pour rechercher le ICAO correspondant dans la table "airports"
        if(!query->exec("SELECT icao FROM airports WHERE iata LIKE '" + nameOfCity + "' OR icao LIKE '" + nameOfCity + "' COLLATE NOCASE")) {

            // Si la requete n'aboutie a aucun résultat, on notifie l'utilisateur
            qDebug() << "Erreur requete icao";
            return "";

        } else {
            // Si la requete n'a pas trouvée, au moins, un résultat
            if(!query->next()) {

                 // On pofine notre recherche en regardant si notre supposé ICAO correspond a un nom d'aeroport ou d'une ville
                if(!query->exec("SELECT icao FROM airports WHERE name LIKE '%" + nameOfCity + "%' OR city LIKE '%" + nameOfCity + "%' COLLATE NOCASE")) {

                    // Si la requete n'aboutie a aucun résultat, on notifie l'utilisateur
                    qDebug() << "Erreur requete icao";
                    return "";

                } else {

                    // Si la requete n'a pas trouvée, au moins, un résultat
                    if(!query->next()) {
                        // Il s'agit d'un "faux" ICAO, on notifie l'utilisateur
                        qDebug() << "Erreur aucun icao correspondant";
                        return "";
                    }
                }
            }
        }
    // Si la taille du parametre nameOfCity ne correspond pas a un code ICAO, il s'agit alors soit d'une ville, soit d'un nom d'aeroport
    } else {

        // On execute alors une requete pour voir si le param nameOfCity correspond a un nom d'aeroport ou d'une ville dans la table "airports"
        if(!query->exec("SELECT icao FROM airports WHERE name LIKE '%" + nameOfCity + "%' OR city LIKE '%" + nameOfCity + "%' COLLATE NOCASE")) {

            // Si la requete n'aboutie a aucun résultat, on notifie l'utilisateur
            qDebug() << "Erreur requete icao";
            return "";
        }

        // Si la requete n'a pas trouvée, au moins, un résultat
        if(!query->next()) {

            // Il s'agit d'un "faux" nom de ville/aeroport, on notifie l'utilisateur
            qDebug() << "Erreur aucun icao correspondant";
            return "";
        }

    }

    // Finalement, si toutes les conditions sont vérifiées, on renvoie le premier résultat de la requete, c.a.d l'icao correspondant au nameOfCity le plus proche dans la bdd "airports"
    return query->value(0).toString();

    // On termine la requete sql
    query->finish();
}


/**
 * @brief @name findSimilarAirport permet de récuperer tous les aeroports dont les codes ICAO/iata, nom de ville/aeroport sont similaires au param nameOfTheCity,
 *
 * @param QString nameOfCity, nom de la ville ou chercher un aéroport
 *
 * @return QStringList, liste de nom d'aeroports similaires trouvés
 */
QStringList DbManager::findSimilarAirport(QString nameOfTheCity) {

    // Initialisation d'une nouvelle requete SQL
    QSqlQuery* query = new QSqlQuery(getDbConnection().getConnection());

    // Requete SQL pour rechercher les noms similaires au param nameOfTheCity dans la table "airports"
    if(!query->exec("SELECT airports.name FROM airports WHERE" + ((nameOfTheCity.size() == 3 || nameOfTheCity.size() == 4) ? (" iata LIKE '" + nameOfTheCity + "' OR icao LIKE '" + nameOfTheCity + "' OR ") : " ") + "airports.city LIKE '%" + nameOfTheCity + "%' OR airports.name LIKE '%" + nameOfTheCity + "%' COLLATE NOCASE")) {

        // Si la requete n'aboutie a aucun résultat, on notifie l'utilisateur
        qDebug() << "Erreur de similitude - " << query->lastError() << " - " << query->executedQuery() ;
        return QStringList();

    }

    // On créée une liste d'aéroports
    QStringList cityAirportList;

    // Tant que la requete a trouvées un/des résultat(s)
    while(query->next()) {
        // On ajoute ce dernier dans la liste des aéroports similaires
        cityAirportList << query->value(0).toString();
    }

    // On renvoie la liste ainsi trouvée
    return cityAirportList;

    // On termine la requete sql
    query->finish();

}
