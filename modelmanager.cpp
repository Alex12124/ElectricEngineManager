#include "modelmanager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <type_traits>
#ifdef DEBUG_MODE
#include <QDebug>
#endif

ModelManager::ModelManager(const QString &path) {
  auto database = QSqlDatabase::addDatabase(
      "QSQLITE", QString::fromStdString(connection_name_));
  database.setDatabaseName(path);
  bool openStatus = database.open();
#ifdef DEBUG_MODE
  qDebug() << "Database connection: " << (openStatus ? "OK" : "Error");
#endif
  QSqlQuery("PRAGMA foreign_keys = ON", database);
}

ModelManager::~ModelManager() {
  if (QSqlDatabase::contains(QString::fromStdString(connection_name_))) {
    QSqlDatabase::database(QString::fromStdString(connection_name_)).close();
  }
}

void ModelManager::DebugQuery(const QSqlQuery &query) {
#ifdef DEBUG_MODE
  if (query.lastError().type() == QSqlError::ErrorType::NoError) {
    qDebug() << "Query OK:" << query.lastQuery();
  } else {
    qWarning() << "Query KO:" << query.lastError().text();
    qWarning() << "Query text:" << query.lastQuery();
  }
#endif
}
