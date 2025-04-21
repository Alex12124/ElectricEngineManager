#include "remote.h"

#include <QTimer>
#include <memory>

#include "message.pb.h"
#include "modelmanager.h"
#include "server/server.h"
#include "ui_remote.h"

namespace pages::remote {

Remote::Remote(ModelManager* model_manager, server::Server* server_ptr,
               QWidget* parent)
    : Page(parent),
      ui_(new Ui::Remote),
      model_manager_(model_manager),
      server_(server_ptr),
      survey_server_timer_(new QTimer(this)) {
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
          &Remote::OnSurveyServer);
}

Remote::~Remote() { delete ui_; }

void Remote::OnPause() { survey_server_timer_->stop(); }
void Remote::OnResume() {
  ui_->pb_server->setChecked(false);
  ui_->pb_server->setText("Отслеживать\nсообщения\nсервера");
  ui_->lbl_status->setText("Статус: сообщения не отслеживаются");
  ui_->pb_back->setEnabled(true);
}

void Remote::OnSurveyServer() {
  if (server_->IsSessionAlive()) {
    Navigate("remote::RemoteRunEngine");
  }
}

void Remote::on_pb_back_clicked() { Back(); }

void Remote::on_pb_server_clicked() {
  if (ui_->pb_server->isChecked()) {
    ui_->pb_server->setText("Не отслеживать\nсообщения\nсервера");
    ui_->lbl_status->setText("Статус: сообщения отслеживаются");
    survey_server_timer_->start(1000);
    ui_->pb_back->setEnabled(false);
  } else {
    survey_server_timer_->stop();
    ui_->pb_server->setText("Отслеживать\nсообщения\nсервера");
    ui_->lbl_status->setText("Статус: сообщения не отслеживаются");
    ui_->pb_back->setEnabled(true);
  }
}

}  // namespace pages::remote
