#include "preparestart.h"

#include <message.pb.h>

#include <QFont>
#include <QModelIndex>
#include <QTimer>
#include <QWidget>
#include <QtConcurrent>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "async_electric_engine.h"
#include "model/engine/engine_model.h"
#include "model/launch/launch_model.h"
#include "modelmanager.h"
#include "pages/page.h"
#include "ui_preparestart.h"

namespace pages::engine {
PrepareStart::PrepareStart(ModelManager* model_manager, QWidget* parent)
    : Page(parent),
      ui_(new Ui::PrepareStart),
      model_manager_(model_manager),
      pull_data_timer_(new QTimer(this)) {
  ui_->setupUi(this);
  for (auto* lbl : findChildren<QLabel*>()) {
    QFont font = lbl->font();
    font.setBold(true);
    lbl->setFont(font);
  }
  for (auto* pb : findChildren<QPushButton*>()) {
    QFont font = pb->font();
    font.setBold(true);
    pb->setFont(font);
  }
  connect(pull_data_timer_, &QTimer::timeout,
          [this]() { PrintEngineParameters(engine_->GetDynamicParameters()); });
}

void PrepareStart::PrintEngineParameters(
    const simulation::electric_engine::DynamicRelativeParameters& rp) {
  ui_->current1->setText(
      QString::number(std::sqrt(rp.I1x * rp.I1x + rp.I1y * rp.I1y), 'f', 4));
  ui_->psi1->setText(QString::number(
      std::sqrt(rp.psi1x * rp.psi1x + rp.psi1y * rp.psi1y), 'f', 4));
  ui_->psi2->setText(QString::number(
      std::sqrt(rp.psi2x * rp.psi2x + rp.psi2y * rp.psi2y), 'f', 4));
  ui_->time->setText(QString::number(rp.total_time / 314.159, 'f', 4));
  ui_->speed->setText(QString::number(rp.speed, 'f', 4));
  ui_->torque->setText(QString::number(rp.torque, 'f', 4));
}

PrepareStart::~PrepareStart() { delete ui_; }

void PrepareStart::OnResume() {
  ui_->pb_apply_voltage->setEnabled(true);
  ui_->current1->setText("0");
  ui_->psi1->setText("0");
  ui_->psi2->setText("0");
  ui_->time->setText("0");
  ui_->speed->setText("0");
  ui_->torque->setText("0");
}

void PrepareStart::OnPause() {
  pull_data_timer_->stop();
  if (engine_) {
    engine_->Stop();
    engine_run_future_.waitForFinished();
    engine_.reset();
  }
}

void PrepareStart::SetData(std::shared_ptr<Transfer> transfer) {
  QModelIndex index;
  auto* model = model_manager_->GetModel<model::EngineModel>();

  index = std::any_cast<QModelIndex>(transfer->at("model_index"));
  engine_data_ = model->data(index, model::EngineModel::Roles::EngineRole)
                     .value<model::Engine>();
  ui_->name->setText(QString::fromStdString(engine_data_.GetName()));
  return;
}

void PrepareStart::on_pb_back_clicked() { Back(); }

void PrepareStart::on_pb_apply_voltage_clicked() {
  QString btn_text = "";
  if (ui_->pb_apply_voltage->isChecked()) {
    btn_text = "Снять питание";
    ui_->pb_back->setEnabled(false);
  } else {
    btn_text = "Подать питание";
    ui_->pb_apply_voltage->setEnabled(false);
    ui_->pb_back->setEnabled(true);
  }
  ui_->pb_apply_voltage->setText(btn_text);
  ManageEngine(ui_->pb_apply_voltage->isChecked());
}

static std::chrono::time_point<std::chrono::system_clock> AddSeconds(
    std::chrono::time_point<std::chrono::system_clock> tp, double seconds) {
  auto sec = std::chrono::duration<double, std::ratio<1>>(seconds);
  auto dur = std::chrono::duration_cast<std::chrono::seconds>(sec);
  return tp + dur;
}

static simulation::electric_engine::Characteristics
ConvertToEngineCharacteristics(const model::Characteristics& c) {
  return {c.power,       c.rated_voltage,     c.rated_current,
          c.rated_speed, c.synchronous_speed, c.starting_torque,
          c.max_torque,  c.moment_of_inertia, c.efficiency,
          c.cos_phi,     c.starting_current};
}

void PrepareStart::ManageEngine(bool is_voltage_up) {
  if (is_voltage_up) {
    double rated_freq = 50;
    simulation::electric_engine::SimulationConfig sc{.delta_time = 0.00001};
    auto load_func = [](double speed) -> double {
      double fan_load = 1.0;  // Вентиляторная нагрузка двигателя
      double friction_load = 0.01;
      return fan_load * speed * speed + friction_load * speed;
    };
    engine_ = std::make_shared<simulation::electric_engine::Asynchronous>(
        rated_freq, sc, std::move(load_func),
        ConvertToEngineCharacteristics(engine_data_.GetСharacteristics()));
    auto engine = engine_;
    launch_data_.SetEngineId(engine_data_.GetId());
    launch_data_.SetEngineName(engine_data_.GetName());
    launch_data_.SetLaunchTime(std::chrono::system_clock::now());
    engine_run_future_ = QtConcurrent::run(QThreadPool::globalInstance(),
                                           [engine]() { engine->Run(); });
    pull_data_timer_->start(50);

  } else {
    assert(engine_.get() != nullptr);
    engine_->SetVoltage(false);
    auto* launch_model = model_manager_->GetModel<model::LaunchModel>();

    launch_data_.SetShutdownTime(
        AddSeconds(launch_data_.GetLaunchTime(),
                   engine_->GetDynamicParameters().total_time / 314.159));
    launch_model->AddLaunch(launch_data_);
  }
}

}  // namespace pages::engine
