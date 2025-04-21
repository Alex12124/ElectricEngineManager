#include "launch_dao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <string>

#include "launch.h"
#include "modelmanager.h"

namespace model {

LaunchDao::LaunchDao(const std::string &connection_name)
    : connection_name_(connection_name) {}

static void BindValues(QSqlQuery &query, const Launch &launch) {
  query.bindValue(":launch_time",
                  QString::fromStdString(launch.GetLaunchTimeStr()));
  query.bindValue(":shutdown_time",
                  QString::fromStdString(launch.GetShutdownTimeStr()));
  query.bindValue(":engine_id", launch.GetEngineId());
}

void LaunchDao::Init() const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  if (!database.tables().contains("launches")) {
    QSqlQuery query(database);
    query.exec(
        "CREATE TABLE launches "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "launch_time TEXT NOT NULL, "
        "shutdown_time TEXT NOT NULL, "
        "engine_id INTEGER NOT NULL, "
        "FOREIGN KEY (engine_id) REFERENCES engines(id) "
        "ON DELETE CASCADE ON UPDATE CASCADE"
        ")");
#ifdef DEBUG_MODE
    ModelManager::DebugQuery(query);
#endif
  }
}

bool LaunchDao::Add(Launch &launch) const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare(
      "INSERT INTO launches "
      "(launch_time,"
      "shutdown_time,"
      "engine_id) "
      "VALUES ("
      ":launch_time,"
      ":shutdown_time,"
      ":engine_id"
      ")");
  BindValues(query, launch);
  bool res = query.exec();
  launch.SetId(query.lastInsertId().toInt());
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

void LaunchDao::Update(const Launch &launch) const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare(
      "UPDATE launches SET launch_time = (:launch_time),"
      "shutdown_time = (:shutdown_time),"
      "engine_id = (:engine_id)"
      "WHERE id = (:id)");
  BindValues(query, launch);
  query.bindValue(":id", launch.GetId());
  query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
}

bool LaunchDao::Remove(std::vector<int> ids) const {
  QStringList id_strings;
  for (int id : ids) {
    id_strings.append(QString::number(id));
  }
  QString placeholders = id_strings.join(", ");
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare("DELETE FROM launches WHERE id IN ((:ids))");
  query.bindValue(":ids", placeholders);
  bool res = query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

bool LaunchDao::RemoveByEngineId(int engine_id) const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare("DELETE FROM launches WHERE engine_id = (:engine_id)");
  query.bindValue(":engine_id", engine_id);
  bool res = query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

std::vector<Launch> LaunchDao::Launches() const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(
      "SELECT launches.id, launches.engine_id, launches.launch_time, "
      "launches.shutdown_time, engines.name FROM launches "
      "JOIN engines ON engines.id = launches.engine_id",
      database);
  query.exec();
  std::vector<Launch> launches;
  while (query.next()) {
    Launch launch;
    launch.SetId(query.value("id").toInt());
    launch.SetEngineId(query.value("engine_id").toInt());
    launch.SetLaunchTime(query.value("launch_time").toString().toStdString());
    launch.SetShutdownTime(
        query.value("shutdown_time").toString().toStdString());
    launch.SetEngineName(query.value("engines.name").toString().toStdString());
    launches.push_back(launch);
  }
  return launches;
}

}  // namespace model
