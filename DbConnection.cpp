#include "DbConnection.h"

DbConnection::DbConnection() {}

/**
 * @brief @class DbConnection permet la gestion de la connexion a la base de donnée QSQLITE :
 * initialisation, connexion, fermeture...
 *
 *  @param QString& chemin, chemin/location du fichier de la bdd
 */
DbConnection::DbConnection(const QString& chemin) {

    database = QSqlDatabase::addDatabase("QSQLITE"); // Ajout de la base de donnée QSQLITE dans le programme
    getConnection().setDatabaseName(chemin); // Initialisation du chemin d'acces a la bdd

    // On notifie l'utilisateur de la connexion ou non avec la bdd
    if(!getConnection().open()) {
        qDebug() << "Erreur : Connexion impossible avec la base de donnée SQLite.";
    } else {
        qDebug() << "Database: Connexion établie avec la base de donnée.";
    }

}

/**
 * @brief @name close permet de fermer la connexion a la base de donnée
 *
 * @param none
 *
 * @return void, none
 */
void DbConnection::close(){
    getConnection().close();
}

/**
 * @brief @name getConnection permet d'avoir acces a la base de donnée
 *
 * @param none
 *
 * @return QSqlDatabase database, renvoie un acces a la bdd
 */
QSqlDatabase DbConnection::getConnection(){
    return database;
}
