#pragma once
#include <QWidget>

#include "modelmanager.h"
#include "pages/page.h"

namespace Ui {
class AddEngine;
}
class ModelManager;

namespace pages::engine {
class AddEngine : public Page {
  Q_OBJECT

 public:
  explicit AddEngine(ModelManager* model_manager, QWidget* parent = nullptr);
  ~AddEngine();

 public:
  void OnResume();

 private:
  bool CheckValues();

 private slots:
  void on_pb_cancel_clicked();

  void on_pb_accept_clicked();

 private:
  Ui::AddEngine* ui_;
  ModelManager* model_manager_;
};
}  // namespace pages::engine
