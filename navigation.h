#pragma once

#include <QObject>
#include <QStackedWidget>
#include <any>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "pages/page.h"

class Navigation : public QObject {
  Q_OBJECT
 public:
  using PageMap = std::unordered_map<std::string, std::pair<pages::Page*, int>>;
  using Transfer = std::unordered_map<std::string, std::any>;
  Navigation(QStackedWidget* stack, PageMap m, const std::string& start_page);

  void ConnectPage(pages::Page* page);
  void DisconnectPage(pages::Page* page);

 public slots:
  void Navigate(const std::string& page);
  void Navigate(const std::string& page, std::shared_ptr<Transfer>);
  void Back();

 private:
  struct PageInfo {
    pages::Page* page;
    int pos;
  };

 private:
  QStackedWidget* ui_stack_;
  PageMap page_id_to_page_;
  std::stack<PageInfo> pos_stack_;
};
