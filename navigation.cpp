#include "navigation.h"

#include <QDebug>
#include <QStackedWidget>
#include <memory>
#include <string>
#include <utility>

#include "pages/page.h"

Navigation::Navigation(QStackedWidget* stack, PageMap m,
                       const std::string& start_page)
    : ui_stack_(stack), page_id_to_page_(std::move(m)) {
  auto [page, pos] = page_id_to_page_.at(start_page);
  pos_stack_.push({page, pos});
  ui_stack_->setCurrentIndex(pos);
  ConnectPage(page);
}

void Navigation::ConnectPage(pages::Page* page) {
  connect(page, &pages::Page::Back, this, &Navigation::Back);
  connect(page, SIGNAL(Navigate(const std::string&)), this,
          SLOT(Navigate(const std::string&)));
  connect(page, SIGNAL(Navigate(const std::string&, std::shared_ptr<Transfer>)),
          this, SLOT(Navigate(const std::string&, std::shared_ptr<Transfer>)));
}

void Navigation::DisconnectPage(pages::Page* page) {
  disconnect(page, &pages::Page::Back, this, &Navigation::Back);
  disconnect(page, SIGNAL(Navigate(const std::string&)), this,
             SLOT(Navigate(const std::string&)));
  disconnect(
      page, SIGNAL(Navigate(const std::string&, std::shared_ptr<Transfer>)),
      this, SLOT(Navigate(const std::string&, std::shared_ptr<Transfer>)));
}

void Navigation::Navigate(const std::string& page) {
  if (page_id_to_page_.count(page) == 0) {
    throw std::logic_error("Invalid page id");
  }
  auto [curr_page, curr_pos] = pos_stack_.top();
  auto [new_page, new_pos] = page_id_to_page_.at(page);
  if (curr_page == new_page) {
    return;
  }
  DisconnectPage(curr_page);
  curr_page->OnPause();
  ConnectPage(new_page);
  pos_stack_.push({new_page, new_pos});
  ui_stack_->setCurrentIndex(new_pos);
  new_page->OnResume();
}

void Navigation::Navigate(const std::string& page,
                          std::shared_ptr<Transfer> data) {
  auto [new_page, new_pos] = page_id_to_page_.at(page);
  new_page->SetData(std::move(data));
  Navigate(page);
}

void Navigation::Back() {
  auto [curr_page, curr_pos] = pos_stack_.top();
  DisconnectPage(curr_page);
  curr_page->OnPause();
  pos_stack_.pop();
  auto [new_page, new_pos] = pos_stack_.top();
  ConnectPage(new_page);
  ui_stack_->setCurrentIndex(new_pos);
  new_page->OnResume();
}
