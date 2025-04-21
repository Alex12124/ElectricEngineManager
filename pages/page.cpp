#include "page.h"

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

namespace pages {

Page::Page(QWidget* parent) : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground);
}

static int CalcFont(int height, QString status) {
  if (status.isEmpty() || status == "medium") {
    return height / 20;
  }
  if (status == "normal") {
    return height / 22;
  }
  if (status == "small") {
    return height / 24;
  }
  return height / 18;
}

void Page::resizeEvent(QResizeEvent* event) {
  for (auto* lbl : findChildren<QLabel*>()) {
    int new_font_size = CalcFont(height(), lbl->statusTip());
    QFont font = lbl->font();
    font.setPixelSize(new_font_size);
    lbl->setFont(font);
  }
  for (auto* le : findChildren<QLineEdit*>()) {
    int new_font_size = CalcFont(height(), le->statusTip());
    QFont font = le->font();
    font.setPixelSize(new_font_size);
    le->setFont(font);
  }
  for (auto* pb : findChildren<QPushButton*>()) {
    int new_font_size = CalcFont(height(), pb->statusTip());
    QFont font = pb->font();
    font.setPixelSize(new_font_size);
    pb->setFont(font);
  }
  QWidget::resizeEvent(event);
}

void Page::SetData(std::shared_ptr<Transfer> data) {}

void Page::OnResume() {}

void Page::OnPause() {}

}  // namespace pages
