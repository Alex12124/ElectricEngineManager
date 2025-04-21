#pragma once
#include <QSqlDatabase>
#include <string>
#include <vector>

#include "launch.h"

namespace model {

class LaunchDao {
 public:
  LaunchDao(const std::string& connection_name);
  ~LaunchDao() = default;

  void Init() const;
  bool Add(Launch& launch) const;
  void Update(const Launch& launch) const;
  bool Remove(std::vector<int> id) const;
  bool RemoveByEngineId(int engine_id) const;

  std::vector<Launch> Launches() const;

 private:
  std::string connection_name_;
};

}  // namespace model
