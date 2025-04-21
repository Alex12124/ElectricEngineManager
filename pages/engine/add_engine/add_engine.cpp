#include "add_engine.h"

#include <QFont>
#include <QToolTip>
#include <QValidator>
#include <QWidget>

#include "model/engine/engine_model.h"
#include "modelmanager.h"
#include "pages/page.h"
#include "ui_add_engine.h"

namespace pages::engine {
AddEngine::AddEngine(ModelManager* model_manager, QWidget* parent)
    : Page(parent), ui_(new Ui::AddEngine), model_manager_(model_manager) {
  ui_->setupUi(this);
  for (auto* lbl : findChildren<QLabel*>()) {
    QFont font = lbl->font();
    font.setBold(true);
    lbl->setFont(font);
  }
  for (auto* le : findChildren<QLineEdit*>()) {
    QFont font = le->font();
    font.setBold(true);
    le->setFont(font);
  }
  for (auto* pb : findChildren<QPushButton*>()) {
    QFont font = pb->font();
    font.setBold(true);
    pb->setFont(font);
  }
  ui_->lb_cos_phi->setText("cosφ");
  auto* v = new QDoubleValidator(this);
  ui_->le_power->setValidator(v);
  ui_->le_rated_voltage->setValidator(v);
  ui_->le_rated_current->setValidator(v);
  ui_->le_rated_speed->setValidator(v);
  ui_->le_cos_phi->setValidator(v);
  ui_->le_sync_speed->setValidator(v);
  ui_->le_starting_torque->setValidator(v);
  ui_->le_max_torque->setValidator(v);
  ui_->le_inertia->setValidator(v);
  ui_->le_efficiency->setValidator(v);
  ui_->le_starting_current->setValidator(v);
}

AddEngine::~AddEngine() { delete ui_; }

void AddEngine::OnResume() {}

void AddEngine::on_pb_cancel_clicked() { Back(); }

bool AddEngine::CheckValues() {
  std::vector<QLineEdit*> line_edits = {ui_->le_name,
                                        ui_->le_power,
                                        ui_->le_rated_voltage,
                                        ui_->le_rated_current,
                                        ui_->le_rated_speed,
                                        ui_->le_sync_speed,
                                        ui_->le_starting_torque,
                                        ui_->le_max_torque,
                                        ui_->le_inertia,
                                        ui_->le_efficiency,
                                        ui_->le_cos_phi,
                                        ui_->le_starting_current};
  return std::all_of(line_edits.begin(), line_edits.end(),
                     [](QLineEdit* le) { return !le->text().isEmpty(); });
}

void AddEngine::on_pb_accept_clicked() {
  if (!CheckValues()) {
    QToolTip::showText(QCursor::pos(), "Введите все поля", this);
    return;
  }
  QLocale locale;
  auto model = model_manager_->GetModel<model::EngineModel>();
  auto ch =
      model::Characteristics{locale.toDouble(ui_->le_power->text()),
                             locale.toDouble(ui_->le_rated_voltage->text()),
                             locale.toDouble(ui_->le_rated_current->text()),
                             locale.toDouble(ui_->le_rated_speed->text()),
                             locale.toDouble(ui_->le_sync_speed->text()),
                             locale.toDouble(ui_->le_starting_torque->text()),
                             locale.toDouble(ui_->le_max_torque->text()),
                             locale.toDouble(ui_->le_inertia->text()),
                             locale.toDouble(ui_->le_efficiency->text()),
                             locale.toDouble(ui_->le_cos_phi->text()),
                             locale.toDouble(ui_->le_starting_current->text())};
  model::Engine engine(ch);

  engine.SetName(ui_->le_name->text().toStdString());
  bool res = model->AddEngine(engine);
  if (!res) {
    QToolTip::showText(QCursor::pos(), "Вставка в базу неудачна", this);
    return;
  }
  Back();
}

}  // namespace pages::engine
