#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <DbConnection.h>
#include <Includes.h>

class DbManager
{
public:
    DbManager();

    DbConnection getDbConnection();
    QString lookForICAO(QString nameOfCity);
    QStringList findSimilarAirport(QString nameOfTheCity);


private:

    DbConnection connexion;
    QSqlQuery *query;

};

#endif // DBMANAGER_H
