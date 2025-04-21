#include "engine_dao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <cassert>
#include <vector>

#include "engine.h"
#include "modelmanager.h"

namespace model {

EngineDao::EngineDao(const std::string &connection_name)
    : connection_name_(connection_name) {}

void EngineDao::Init() const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  if (!database.tables().contains("engines")) {
    QSqlQuery query(database);
    query.exec(
        "CREATE TABLE engines "
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "power REAL NOT NULL, "
        "rated_voltage REAL NOT NULL, "
        "rated_current REAL NOT NULL, "
        "rated_speed REAL NOT NULL, "
        "synchronous_speed REAL NOT NULL, "
        "starting_torque REAL NOT NULL, "
        "max_torque REAL NOT NULL, "
        "moment_of_inertia REAL NOT NULL, "
        "efficiency REAL NOT NULL, "
        "cos_phi REAL NOT NULL, "
        "starting_current REAL NOT NULL)");
#ifdef DEBUG_MODE
    ModelManager::DebugQuery(query);
#endif
  }
}

static void BindValues(QSqlQuery &query, const Engine &engine) {
  auto ch = engine.GetСharacteristics();
  query.bindValue(":name", QString::fromStdString(engine.GetName()));
  query.bindValue(":power", ch.power);
  query.bindValue(":rated_voltage", ch.rated_voltage);
  query.bindValue(":rated_current", ch.rated_current);
  query.bindValue(":rated_speed", ch.rated_speed);
  query.bindValue(":synchronous_speed", ch.synchronous_speed);
  query.bindValue(":starting_torque", ch.starting_torque);
  query.bindValue(":max_torque", ch.max_torque);
  query.bindValue(":moment_of_inertia", ch.moment_of_inertia);
  query.bindValue(":efficiency", ch.efficiency);
  query.bindValue(":cos_phi", ch.cos_phi);
  query.bindValue(":starting_current", ch.starting_current);
}

bool EngineDao::Add(Engine &engine) const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare(
      "INSERT INTO engines "
      "(name,"
      "power,"
      "rated_voltage,"
      "rated_current,"
      "rated_speed,"
      "synchronous_speed,"
      "starting_torque,"
      "max_torque,"
      "moment_of_inertia,"
      "efficiency,"
      "cos_phi,"
      "starting_current)"
      " VALUES (:name,"
      ":power,"
      ":rated_voltage,"
      ":rated_current,"
      ":rated_speed,"
      ":synchronous_speed,"
      ":starting_torque,"
      ":max_torque,"
      ":moment_of_inertia,"
      ":efficiency,"
      ":cos_phi,"
      ":starting_current"
      ")");
  BindValues(query, engine);
  bool res = query.exec();
  engine.SetId(query.lastInsertId().toInt());
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

void EngineDao::Update(const Engine &engine) const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare(
      "UPDATE engines SET name = (:name),"
      "power = (:power),"
      "rated_voltage = (:rated_voltage),"
      "rated_current = (:rated_current),"
      "rated_speed = (:rated_speed),"
      "synchronous_speed = (:synchronous_speed),"
      "starting_torque = (:starting_torque),"
      "max_torque = (:max_torque),"
      "moment_of_inertia = (:moment_of_inertia),"
      "efficiency = (:efficiency),"
      "cos_phi = (:cos_phi),"
      "starting_current = (:starting_current)"
      " WHERE id = (:id)");
  BindValues(query, engine);
  query.bindValue(":id", engine.GetId());
  query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
}

bool EngineDao::Remove(int id) const {
  QSqlQuery query;
  query.prepare("DELETE FROM engines WHERE id = (:id)");
  query.bindValue(":id", id);
  bool res = query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

bool EngineDao::Remove(std::vector<int> ids) const {
  QStringList id_strings;
  for (int id : ids) {
    id_strings.append(QString::number(id));
  }
  QString placeholders = id_strings.join(", ");
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query(database);
  query.prepare("DELETE FROM engines WHERE id IN ((:ids))");
  query.bindValue(":ids", placeholders);
  bool res = query.exec();
#ifdef DEBUG_MODE
  ModelManager::DebugQuery(query);
#endif
  return res;
}

std::vector<Engine> EngineDao::Engines() const {
  auto database =
      QSqlDatabase::database(QString::fromStdString(connection_name_));
  QSqlQuery query("SELECT * FROM engines", database);
  query.exec();
  std::vector<Engine> engines;
  while (query.next()) {
    Characteristics ch{
        query.value("power").toDouble(),
        query.value("rated_voltage").toDouble(),
        query.value("rated_current").toDouble(),
        query.value("rated_speed").toDouble(),
        query.value("synchronous_speed").toDouble(),
        query.value("starting_torque").toDouble(),
        query.value("max_torque").toDouble(),
        query.value("moment_of_inertia").toDouble(),
        query.value("efficiency").toDouble(),
        query.value("cos_phi").toDouble(),
        query.value("starting_current").toDouble(),
    };
    Engine engine = Engine(ch);
    engine.SetId(query.value("id").toInt());
    engine.SetName(query.value("name").toString().toStdString());
    engines.push_back(engine);
  }
  return engines;
}

}  // namespace model
