#pragma once

#include <QString>
#include <any>
#include <memory>
#include <string>
#include <unordered_map>

#include "model/engine/engine_dao.h"
#include "model/engine/engine_model.h"
#include "model/launch/launch_model.h"

class QSqlDatabase;
class QSqlQuery;

class ModelManager {
 public:
  ModelManager(const QString& path = "engine.sqlite");
  ~ModelManager();
  static void DebugQuery(const QSqlQuery& query);

  template <typename T>
  T* GetModel();

 private:
  std::unordered_map<std::string, std::any> models_;
  std::string connection_name_ = "main_connection";
};

template <typename T>
T* ModelManager::GetModel() {
  if constexpr (std::is_same_v<T, model::EngineModel>) {
    if (models_.count("model::EngineModel") > 0) {
      return std::any_cast<std::shared_ptr<model::EngineModel>>(
                 models_.at("model::EngineModel"))
          .get();
    } else {
      auto engine_dao = std::make_shared<model::EngineDao>(connection_name_);
      auto engine_model = std::make_shared<model::EngineModel>(engine_dao);
      models_["model::EngineModel"] = engine_model;
      return engine_model.get();
    }
  } else if constexpr (std::is_same_v<T, model::LaunchModel>) {
    if (models_.count("model::LaunchModel") > 0) {
      return std::any_cast<std::shared_ptr<model::LaunchModel>>(
                 models_.at("model::LaunchModel"))
          .get();
    } else {
      auto launch_dao = std::make_shared<model::LaunchDao>(connection_name_);
      auto launch_model = std::make_shared<model::LaunchModel>(launch_dao);
      models_["model::LaunchModel"] = launch_model;
      return launch_model.get();
    }
  }
}
