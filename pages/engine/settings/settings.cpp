#include "settings.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QToolTip>

#include "./ui_settings.h"
#include "model/engine/engine_model.h"
#include "modelmanager.h"
#include "pages/page.h"

namespace pages::engine {
Settings::Settings(ModelManager* model_manager, QWidget* parent)
    : Page(parent), ui_(new Ui::Settings), model_manager_(model_manager) {
  ui_->setupUi(this);
  auto proxy_model = new QSortFilterProxyModel(this);
  ui_->engines_view->setSortingEnabled(true);
  ui_->engines_view->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  proxy_model->setSourceModel(model_manager->GetModel<model::EngineModel>());
  ui_->engines_view->setModel(proxy_model);
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
  ui_->engines_view->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {font-size: 16px;"
      "font-weight:bold;"
      "font-family:\"DejaVu Serif\";}");
}

Settings::~Settings() { delete ui_; }

void Settings::OnResume() {}

void Settings::on_pb_next_clicked() {
  auto* selection_model = ui_->engines_view->selectionModel();
  auto* model = ui_->engines_view->model();
  if (selection_model->selectedIndexes().isEmpty()) {
    QToolTip::showText(QCursor::pos(), "Выберите двигатель", this);
    return;
  }

  QModelIndex model_index =
      static_cast<QSortFilterProxyModel*>(model)->mapToSource(
          selection_model->currentIndex());

  auto transfer = std::make_shared<Transfer>();
  transfer->insert({"model_index", model_index});
  Navigate("engine::PrepareStart", std::move(transfer));
}

void Settings::on_pb_back_clicked() { Back(); }

void Settings::on_pb_add_clicked() { Navigate("engine::AddEngine"); }

void Settings::on_pb_remove_clicked() {
  auto* selection_model = ui_->engines_view->selectionModel();
  auto* model = ui_->engines_view->model();
  auto* launch_model = model_manager_->GetModel<model::LaunchModel>();
  if (!selection_model->selectedIndexes().isEmpty()) {
    QMessageBox msg_box;
    msg_box.setText("Точно хотите удалить двигатель?");
    msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg_box.setWindowTitle("Удаление двигателя");
    int ret = msg_box.exec();
    if (ret == QMessageBox::Ok) {
      int row = selection_model->currentIndex().row();
      model->removeRows(row, 1, QModelIndex());
      launch_model->Synchronize();
    }
  } else {
    QToolTip::showText(QCursor::pos(), "Выберите двигатель", this);
  }
}

}  // namespace pages::engine
