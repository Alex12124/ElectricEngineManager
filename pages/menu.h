#pragma once
#include <QWidget>

#include "modelmanager.h"
#include "page.h"

namespace Ui {
class Menu;
}

namespace pages {

class Menu : public Page {
  Q_OBJECT

 public:
  explicit Menu(ModelManager* model_manager, QWidget* parent = nullptr);

 public:
  void OnResume();
  ~Menu();

 private slots:
  void on_pb_to_engine_clicked();

  void on_pb_to_logs_clicked();

  void on_pb_remote_clicked();

 private:
  Ui::Menu* ui_;
  ModelManager* model_manager_;
};
}  // namespace pages
