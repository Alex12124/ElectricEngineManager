#include "launch_log.h"

#include <QSortFilterProxyModel>

#include "model/launch/launch.h"
#include "modelmanager.h"
#include "ui_launch_log.h"

namespace pages::launch_log {

LaunchLog::LaunchLog(ModelManager* model_manager, QWidget* parent)
    : Page(parent), ui_(new Ui::LaunchLog), model_manager_(model_manager) {
  ui_->setupUi(this);
  auto* proxy_model = new QSortFilterProxyModel(this);
  ui_->launches_view->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  proxy_model->setSourceModel(model_manager->GetModel<model::LaunchModel>());
  proxy_model->setFilterKeyColumn(0);
  proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
  ui_->launches_view->setModel(proxy_model);
  ui_->launches_view->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {font-size: 16px;"
      "font-weight:bold;"
      "font-family:\"DejaVu Serif\";}");
  connect(ui_->le_filter, &QLineEdit::textChanged,
          [proxy_model](const QString& text) {
            proxy_model->setFilterRegularExpression(text);
          });
}

LaunchLog::~LaunchLog() { delete ui_; }

void LaunchLog::OnResume() {}

void LaunchLog::on_pb_back_clicked() { Back(); }

}  // namespace pages::launch_log
