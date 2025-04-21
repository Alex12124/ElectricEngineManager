#include "launch_model.h"

#include <QSqlDatabase>
#include <QSqlQuery>

namespace model {
static const int kColumns = 3;

LaunchModel::LaunchModel(std::shared_ptr<LaunchDao> dao, QObject *parent)
    : QAbstractTableModel(parent), dao_(dao) {
  dao_->Init();
  launches_ = dao_->Launches();
}

QVariant LaunchModel::data(const QModelIndex &index, int role) const {
  if (!IsIndexValid(index)) return QVariant();
  int column = index.column();
  const Launch &launch = launches_.at(index.row());
  switch (role) {
    case Qt::DisplayRole:
      if (column == 0) {
        return QString::fromStdString(launch.GetEngineName());
      } else if (column == 1) {
        return QString::fromStdString(launch.GetLaunchTimeStr());
      } else if (column == 2) {
        return QString::fromStdString(launch.GetShutdownTimeStr());
      }
      return QVariant();
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    default:
      return QVariant();
  }
}

bool LaunchModel::AddLaunch(Launch launch) {
  int row_index = launches_.size();
  bool res = dao_->Add(launch);
  if (res) {
    beginInsertRows(QModelIndex(), row_index, row_index);
    launches_.push_back(launch);
    endInsertRows();
  }
  return res;
}

QVariant LaunchModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch (section) {
        case 0:
          return "Название двигателя";
        case 1:
          return "Время запуска";
        case 2:
          return "Время отключения";
      }
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

int LaunchModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return launches_.size();
}

int LaunchModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return kColumns;
}

bool LaunchModel::removeRows(int row, int count, const QModelIndex &parent) {
  int row_count = launches_.size();
  if (row < 0 || row >= row_count || count <= 0 || (row + count) > row_count) {
    return false;
  }
  std::vector<int> ids(count, 0);

  for (int i = 0; i < count; ++i) {
    const auto &launch = launches_.at(row + i);
    ids[i] = launch.GetId();
  }
  bool res = dao_->Remove(std::move(ids));
  if (res) {
    beginRemoveRows(parent, row, row + count - 1);
    launches_.erase(launches_.begin() + row, launches_.begin() + row + count);
    endRemoveRows();
  }
  return res;
}

void LaunchModel::Synchronize() {
  beginResetModel();
  launches_ = dao_->Launches();
  endResetModel();
}

bool LaunchModel::IsIndexValid(const QModelIndex &index) const {
  if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
    return false;
  }
  return true;
}

}  // namespace model
