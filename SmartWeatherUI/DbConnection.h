#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <Includes.h>

class DbConnection {

public:

    DbConnection();
    DbConnection(const QString& chemin);

    void close();
    QSqlDatabase getConnection();

private:

    QSqlDatabase database;

};

#endif // DBCONNECTION_H
