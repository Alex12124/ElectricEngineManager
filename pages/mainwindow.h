#pragma once

#include <QMainWindow>
#include <QWidget>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "modelmanager.h"
#include "navigation.h"
#include "pages/page.h"
#include "server/server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Main;
}
QT_END_NAMESPACE

namespace pages {

class Main : public QMainWindow {
  Q_OBJECT

 public:
  Main(QWidget *parent = nullptr);
  ~Main();

 private:
  std::vector<std::pair<std::string, Page *>> BuildPages();
  void BuildNavigation(std::vector<std::pair<std::string, Page *>> pages);

 private:
  std::unique_ptr<Navigation> nav_;
  std::unique_ptr<ModelManager> model_manager_;
  std::unique_ptr<server::Server> server_;

  Ui::Main *ui_;
};

}  // namespace pages
