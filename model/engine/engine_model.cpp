#include "engine_model.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <memory>
#include <vector>

#include "engine.h"
#include "engine_dao.h"

namespace model {
static const int kColumns = 6;

EngineModel::EngineModel(std::shared_ptr<EngineDao> dao, QObject *parent)
    : QAbstractTableModel(parent), dao_(std::move(dao)) {
  dao_->Init();
  engines_ = dao_->Engines();
}

bool EngineModel::AddEngine(Engine engine) {
  int row_index = engines_.size();
  bool res = dao_->Add(engine);
  if (res) {
    beginInsertRows(QModelIndex(), row_index, row_index);
    engines_.push_back(engine);
    endInsertRows();
  }
  return res;
}

std::optional<Engine> EngineModel::GetEngine(const std::string &name) {
  auto it = std::find_if(
      engines_.begin(), engines_.end(),
      [&name](const Engine &engine) { return engine.GetName() == name; });
  if (it != engines_.end()) {
    return *it;
  }
  return std::nullopt;
}

int EngineModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return engines_.size();
}

int EngineModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return kColumns;
}

QVariant EngineModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  switch (role) {
    case Qt::DisplayRole:
      if (orientation == Qt::Horizontal) {
        switch (section) {
          case 0:
            return "Название";
          case 1:
            return "P, Вт";
          case 2:
            return "U ном., В";
          case 3:
            return "I ном., А";
          case 4:
            return "n, об/мин";
          case 5:
            return "Пусковой ток, отн. ед.";
        }
      }
    case Roles::NameRole:
      return "Название";
    case Roles::PowerRole:
      return "P, Вт";
    case Roles::VoltageRole:
      return "U ном., В";
    case Roles::CurrentRole:
      return "I ном., А";
    case Roles::SpeedRole:
      return "n, об/мин";
    case Roles::StartingCurrentRole:
      return "Пусковой ток, отн. ед.";
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant EngineModel::data(const QModelIndex &index, int role) const {
  if (!IsIndexValid(index)) return QVariant();
  int column = index.column();
  const Engine &en = engines_.at(index.row());
  const auto &ch = en.GetСharacteristics();
  switch (role) {
    case Qt::DisplayRole:
      if (column == 0) {
        return QString::fromStdString(en.GetName());
      } else if (column == 1) {
        return ch.power;
      } else if (column == 2) {
        return ch.rated_voltage;
      } else if (column == 3) {
        return ch.rated_current;
      } else if (column == 4) {
        return ch.rated_speed;
      } else if (column == 5) {
        return ch.starting_current;
      }
      return QVariant();
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Roles::EngineRole:
      return QVariant::fromValue(en);
    case Roles::NameRole:
      return QString(en.GetName().data());
    case Roles::PowerRole:
      return ch.power;
    case Roles::VoltageRole:
      return ch.rated_voltage;
    case Roles::CurrentRole:
      return ch.rated_current;
    case Roles::SpeedRole:
      return ch.rated_speed;
    case Roles::StartingCurrentRole:
      return ch.starting_current;
    default:
      return QVariant();
  }
}

bool EngineModel::removeRows(int row, int count, const QModelIndex &parent) {
  int row_count = engines_.size();
  if (row < 0 || row >= row_count || count <= 0 || (row + count) > row_count) {
    return false;
  }
  std::vector<int> ids(count, 0);

  for (int i = 0; i < count; ++i) {
    const auto &engine = engines_.at(row + i);
    ids[i] = engine.GetId();
  }
  bool res = dao_->Remove(std::move(ids));
  if (res) {
    beginRemoveRows(parent, row, row + count - 1);
    engines_.erase(engines_.begin() + row, engines_.begin() + row + count);
    endRemoveRows();
  }
  return res;
}

QHash<int, QByteArray> EngineModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles.insert(Roles::NameRole, "name");
  roles.insert(Roles::PowerRole, "power");
  roles.insert(Roles::VoltageRole, "voltage");
  roles.insert(Roles::CurrentRole, "current");
  roles.insert(Roles::SpeedRole, "speed");
  roles.insert(Roles::StartingCurrentRole, "starting_current");
  return roles;
}

bool EngineModel::IsIndexValid(const QModelIndex &index) const {
  if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
    return false;
  }
  return true;
}
}  // namespace model
