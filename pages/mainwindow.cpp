#include "mainwindow.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "./ui_mainwindow.h"
#include "engine/add_engine/add_engine.h"
#include "engine/prepare_start/preparestart.h"
#include "engine/settings/settings.h"
#include "launch_log/launch_log.h"
#include "menu.h"
#include "modelmanager.h"
#include "navigation.h"
#include "pages/page.h"
#include "remote/remote.h"
#include "remote/remote_run_engine.h"
#include "server/server.h"

namespace pages {
Main::Main(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::Main),
      model_manager_(std::make_unique<ModelManager>("engine.sqlite")),
      server_(std::make_unique<server::Server>()) {
  ui_->setupUi(this);
  auto* vbox = new QVBoxLayout();
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->addWidget(ui_->stackedWidget);
  ui_->centralwidget->setLayout(vbox);
  auto pages = BuildPages();
  BuildNavigation(std::move(pages));
  server_->Run();
}

void Main::BuildNavigation(std::vector<std::pair<std::string, Page*>> pages) {
  std::unordered_map<std::string, std::pair<Page*, int>> page_str_to_position;
  for (auto [page_id, page] : pages) {
    int pos = ui_->stackedWidget->addWidget(page);
    if (page_str_to_position.count(page_id) > 0) {
      throw std::logic_error("Page id must be unique");
    }
    page_str_to_position.insert({page_id, {page, pos}});
  }
  nav_ = std::make_unique<Navigation>(ui_->stackedWidget,
                                      std::move(page_str_to_position), "Menu");
}

std::vector<std::pair<std::string, Page*>> Main::BuildPages() {
  std::vector<std::pair<std::string, Page*>> pages;

  pages.push_back({"Menu", new pages::Menu(model_manager_.get(), this)});
  pages.push_back({"engine::Settings",
                   new pages::engine::Settings(model_manager_.get(), this)});
  pages.push_back({"engine::PrepareStart", new pages::engine::PrepareStart(
                                               model_manager_.get(), this)});
  pages.push_back({"engine::AddEngine",
                   new pages::engine::AddEngine(model_manager_.get(), this)});
  pages.push_back({"launch_log::LaunchLog",
                   new launch_log::LaunchLog(model_manager_.get(), this)});

  pages.push_back({"remote::Remote", new remote::Remote(model_manager_.get(),
                                                        server_.get(), this)});
  pages.push_back(
      {"remote::RemoteRunEngine",
       new remote::RemoteRunEngine(model_manager_.get(), server_.get(), this)});

  return pages;
}

Main::~Main() { delete ui_; }

}  // namespace pages
