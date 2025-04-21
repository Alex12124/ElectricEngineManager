#pragma once

#include <QAbstractTableModel>
#include <QObject>
#include <memory>
#include <string>
#include <vector>

#include "launch.h"
#include "launch_dao.h"

namespace model {

class LaunchModel : public QAbstractTableModel {
 public:
  LaunchModel(std::shared_ptr<LaunchDao> dao, QObject *parent = nullptr);
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  bool AddLaunch(Launch launch);
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent) const override;
  bool removeRows(int row, int count, const QModelIndex &parent) override;
  void Synchronize();

 private:
  bool IsIndexValid(const QModelIndex &index) const;

 private:
  std::shared_ptr<LaunchDao> dao_;
  std::vector<Launch> launches_;
};

}  // namespace model
