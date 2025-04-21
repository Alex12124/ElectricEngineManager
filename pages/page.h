#pragma once

#include <QWidget>
#include <any>
#include <memory>
#include <string>
#include <unordered_map>

namespace pages {
class Page : public QWidget {
  Q_OBJECT
 public:
  using Transfer = std::unordered_map<std::string, std::any>;
  explicit Page(QWidget* parent = nullptr);
  void resizeEvent(QResizeEvent* event) override;
 signals:
  void Navigate(const std::string& page);
  void Navigate(const std::string& page, std::shared_ptr<Transfer>);
  void Back();

 public:
  virtual void SetData(std::shared_ptr<Transfer> data);
  virtual void OnResume();
  virtual void OnPause();
};

}  // namespace pages
