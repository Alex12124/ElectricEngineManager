#pragma once

#include <QWidget>

#include "modelmanager.h"
#include "pages/page.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Settings;
}
QT_END_NAMESPACE

class ModelManager;

namespace pages::engine {
class Settings : public Page {
  Q_OBJECT
 public:
  explicit Settings(ModelManager* model_manager, QWidget* parent = nullptr);

  ~Settings();
  // Page interface
 public:
  void OnResume() override;

 private slots:
  void on_pb_next_clicked();

  void on_pb_back_clicked();

  void on_pb_add_clicked();

  void on_pb_remove_clicked();

 private:
  Ui::Settings* ui_;
  ModelManager* model_manager_;
};
}  // namespace pages::engine
