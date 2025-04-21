#include "menu.h"

#include "modelmanager.h"
#include "ui_menu.h"

namespace pages {

Menu::Menu(ModelManager* model_manager, QWidget* parent)
    : Page(parent), ui_(new Ui::Menu), model_manager_(model_manager) {
  ui_->setupUi(this);
}

void Menu::OnResume() {}

Menu::~Menu() { delete ui_; }

void Menu::on_pb_to_engine_clicked() { Navigate("engine::Settings"); }

void Menu::on_pb_to_logs_clicked() { Navigate("launch_log::LaunchLog"); }

void Menu::on_pb_remote_clicked() { Navigate("remote::Remote"); }

}  // namespace pages
