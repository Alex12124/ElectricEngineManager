#pragma once
#include <QMetaType>
#include <string>

namespace model {

struct Characteristics {
  double power;
  double rated_voltage;
  double rated_current;
  double rated_speed;
  double synchronous_speed;
  double starting_torque;
  double max_torque;
  double moment_of_inertia;
  double efficiency;
  double cos_phi;
  double starting_current;
};

class Engine {
 public:
  Engine() = default;
  Engine(const Engine& en) = default;
  explicit Engine(const Characteristics& ch);
  ~Engine() = default;

  Characteristics GetСharacteristics() const;
  void SetСharacteristics(const Characteristics& ch);

  void SetId(int id);
  int GetId() const;

  void SetName(const std::string& name);
  std::string GetName() const;

 private:
  Characteristics characteristics_;
  std::string name_;
  int id_;
};

}  // namespace model
Q_DECLARE_METATYPE(model::Engine);
