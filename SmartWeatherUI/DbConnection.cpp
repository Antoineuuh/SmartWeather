#include "DbConnection.h"


DbConnection::DbConnection() {}

DbConnection::DbConnection(const QString& chemin) {

    database = QSqlDatabase::addDatabase("QSQLITE");
    getConnection().setDatabaseName(chemin);

    if(!getConnection().open()) {

        qDebug() << "Erreur : Connexion impossible avec la base de donnée SQLite.";

    } else {

        qDebug() << "Database: Connexion établie avec la base de donnée.";

    }

}

void DbConnection::close(){

    getConnection().close();

}

QSqlDatabase DbConnection::getConnection(){
    return database;
}
