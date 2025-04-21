#pragma once

#include <QTimer>
#include <QWidget>
#include <memory>

#include "modelmanager.h"
#include "pages/page.h"
#include "server/server.h"
namespace Ui {
class Remote;
}

namespace pages::remote {

class Remote : public Page {
  Q_OBJECT

 public:
  explicit Remote(ModelManager* model_manager, server::Server* server_ptr,
                  QWidget* parent = nullptr);
  ~Remote();

  void OnResume() override;
  void OnPause() override;

 private slots:
  void on_pb_back_clicked();
  void OnSurveyServer();
  void on_pb_server_clicked();

 private:
  Ui::Remote* ui_;
  ModelManager* model_manager_;
  server::Server* server_;
  QTimer* survey_server_timer_;
};

}  // namespace pages::remote
