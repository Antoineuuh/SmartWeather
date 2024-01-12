#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <Includes.h>

class DbConnection {

public:

    // Constructeurs
    DbConnection();
    DbConnection(const QString& chemin);

    // Getter
    QSqlDatabase getConnection();

    // Méthodes spécifiques
    void close();

private:

    QSqlDatabase database;

};

#endif // DBCONNECTION_H
