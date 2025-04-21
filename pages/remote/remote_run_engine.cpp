#include "remote_run_engine.h"

#include <QString>
#include <QtConcurrent>
#include <chrono>
#include <memory>

#include "modelmanager.h"
#include "server/server.h"
#include "ui_remote_run_engine.h"

namespace pages::remote {

RemoteRunEngine::RemoteRunEngine(ModelManager* model_manager,
                                 server::Server* server_ptr, QWidget* parent)
    : Page(parent),
      ui_(new Ui::RemoteRunEngine),
      model_manager_(model_manager),
      server_(server_ptr),
      survey_server_timer_(new QTimer(this)),
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

  connect(survey_server_timer_, &QTimer::timeout, this,
          &RemoteRunEngine::OnSurveyServer);
  connect(pull_data_timer_, &QTimer::timeout,
          [this]() { PrintEngineParameters(engine_->GetDynamicParameters()); });
}

void RemoteRunEngine::PrintEngineParameters(
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

RemoteRunEngine::~RemoteRunEngine() { delete ui_; }

void RemoteRunEngine::OnResume() {
  ui_->lbl_status->setText("Статус:");
  survey_server_timer_->start(1000);
}

void RemoteRunEngine::OnPause() {
  survey_server_timer_->stop();
  pull_data_timer_->stop();
  if (engine_) {
    engine_->Stop();  // Stop simulation != Command stop from server
                      // Command stop set voltage down, but simulation goes
    engine_run_future_.waitForFinished();
    engine_.reset();
  }
}

void RemoteRunEngine::on_pb_back_clicked() { Back(); }

static message::Response GetStartResponse(message::StartStatus status) {
  message::Response response;
  auto* start_response = new message::StartResponse;
  start_response->set_status(status);
  response.set_allocated_start_res(start_response);
  return response;
}

static message::Response GetStopResponse(bool stopped) {
  message::Response response;
  auto* stop_response = new message::StopResponse;
  stop_response->set_stopped(stopped);
  response.set_allocated_stop_res(stop_response);
  return response;
}

void RemoteRunEngine::OnSurveyServer() {
  if (server_->IsSessionAlive()) {
    auto* model = model_manager_->GetModel<model::EngineModel>();
    std::optional<message::Request> maybe_message = server_->TryGetRequest();
    if (!maybe_message.has_value()) {
      return;
    }
    if (maybe_message->has_start_req()) {
      if (engine_ && engine_->IsVoltageUp()) {
        auto response = GetStartResponse(message::StartStatus::kAlreadyStarted);
        server_->SendResponse(response);
        return;
      }

      auto req = maybe_message->start_req();
      auto maybe_engine = model->GetEngine(req.engine_name());
      if (maybe_engine) {
        engine_data_ = *maybe_engine;
        ui_->name->setText(QString::fromStdString(engine_data_.GetName()));
      } else {
        auto response =
            GetStartResponse(message::StartStatus::kNoSuchEngineError);
        server_->SendResponse(response);
        return;
      }
      RunElectricEngine();
      auto response = GetStartResponse(message::StartStatus::kOk);
      server_->SendResponse(response);
      return;
    } else if (maybe_message->has_stop_req()) {
      if (engine_ && engine_->IsRunning()) {
        if (engine_->IsVoltageUp()) {
          StopElectricEngine();
          auto response = GetStopResponse(true);
          server_->SendResponse(response);
          return;
        }
      }
      auto response = GetStopResponse(false);
      server_->SendResponse(response);
    }
  } else {
    Back();
  }
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

void RemoteRunEngine::RunElectricEngine() {
  if (engine_) {
    pull_data_timer_->stop();
    engine_->Stop();  // Command stop engine from server is voltage down.
                      // But simulations goes. engine_->Stop() means simulation
                      // stop.
    engine_run_future_.waitForFinished();
  }
  double rated_freq = 50;
  simulation::electric_engine::SimulationConfig sc{.delta_time = 0.00001};
  auto load_func = [](double speed) -> double {
    double fan_load = 1.0;
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
  ui_->lbl_status->setText("Статус: подано напряжение на двигатель");
  pull_data_timer_->start(50);
}

void RemoteRunEngine::StopElectricEngine() {
  engine_->SetVoltage(false);
  auto* launch_model = model_manager_->GetModel<model::LaunchModel>();

  launch_data_.SetShutdownTime(
      AddSeconds(launch_data_.GetLaunchTime(),
                 engine_->GetDynamicParameters().total_time / 314.159));
  launch_model->AddLaunch(launch_data_);
  ui_->lbl_status->setText("Статус: снято напряжение с двигателя");
}

}  // namespace pages::remote
