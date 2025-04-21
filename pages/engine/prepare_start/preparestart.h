#pragma once

#include <QFuture>
#include <QModelIndex>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "async_electric_engine.h"
#include "model/engine/engine.h"
#include "model/launch/launch.h"
#include "modelmanager.h"
#include "pages/page.h"

namespace Ui {
class PrepareStart;
}

namespace pages::engine {
class PrepareStart : public Page {
  Q_OBJECT

 public:
  explicit PrepareStart(ModelManager* model_manager, QWidget* parent = nullptr);
  ~PrepareStart();
  // Page interface
 public:
  void OnResume() override;
  void OnPause() override;
  void SetData(std::shared_ptr<Transfer>) override;
  void PrintEngineParameters(
      const simulation::electric_engine::DynamicRelativeParameters&);

 private:
  void ManageEngine(bool is_voltage_up);
 private slots:

  void on_pb_back_clicked();
  void on_pb_apply_voltage_clicked();

 private:
  Ui::PrepareStart* ui_;
  ModelManager* model_manager_;
  model::Engine engine_data_;
  std::shared_ptr<simulation::electric_engine::Asynchronous> engine_;

  QTimer* pull_data_timer_;
  model::Launch launch_data_;
  QFuture<void> engine_run_future_;
};
}  // namespace pages::engine
