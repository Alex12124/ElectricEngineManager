#pragma once

#include <QWidget>

#include "modelmanager.h"
#include "pages/page.h"
namespace Ui {
class LaunchLog;
}

namespace pages::launch_log {

class LaunchLog : public Page {
  Q_OBJECT

 public:
  explicit LaunchLog(ModelManager* model_manager, QWidget* parent = nullptr);
  ~LaunchLog();

  void OnResume() override;

 private slots:
  void on_pb_back_clicked();

 private:
  Ui::LaunchLog* ui_;
  ModelManager* model_manager_;
};

}  // namespace pages::launch_log
