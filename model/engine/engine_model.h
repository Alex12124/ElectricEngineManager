#pragma once
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <memory>
#include <optional>
#include <vector>

#include "engine.h"
#include "engine_dao.h"

namespace model {

class EngineModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  enum Roles {
    EngineRole = Qt::UserRole + 1,
    NameRole,
    PowerRole,
    VoltageRole,
    CurrentRole,
    SpeedRole,
    StartingCurrentRole
  };

  explicit EngineModel(std::shared_ptr<EngineDao> dao,
                       QObject *parent = nullptr);
  bool AddEngine(Engine engine);
  std::optional<Engine> GetEngine(const std::string &name);
  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  bool removeRows(int row, int count, const QModelIndex &parent) override;

  QHash<int, QByteArray> roleNames() const;

 private:
  bool IsIndexValid(const QModelIndex &index) const;

 private:
  std::shared_ptr<EngineDao> dao_;
  std::vector<Engine> engines_;
};
}  // namespace model
