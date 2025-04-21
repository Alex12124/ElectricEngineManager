#pragma once

#include <QFuture>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "async_electric_engine.h"
#include "model/engine/engine.h"
#include "model/launch/launch.h"
#include "modelmanager.h"
#include "pages/page.h"
#include "server/server.h"

namespace Ui {
class RemoteRunEngine;
}

namespace pages::remote {

class RemoteRunEngine : public Page {
  Q_OBJECT

 public:
  explicit RemoteRunEngine(ModelManager* model_manager,
                           server::Server* server_ptr,
                           QWidget* parent = nullptr);
  ~RemoteRunEngine();

  void OnResume() override;
  void OnPause() override;
  void RunElectricEngine();
  void StopElectricEngine();

 private:
  void PrintEngineParameters(
      const simulation::electric_engine::DynamicRelativeParameters&);
 private slots:
  void on_pb_back_clicked();
  void OnSurveyServer();

 private:
  Ui::RemoteRunEngine* ui_;
  ModelManager* model_manager_;
  server::Server* server_;
  QTimer* survey_server_timer_;

  model::Engine engine_data_;
  std::shared_ptr<simulation::electric_engine::Asynchronous> engine_;
  model::Launch launch_data_;
  QTimer* pull_data_timer_;
  QFuture<void> engine_run_future_;
};

}  // namespace pages::remote
