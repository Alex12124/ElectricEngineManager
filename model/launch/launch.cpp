#include "launch.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
namespace model {

int Launch::GetId() const { return id_; }

void Launch::SetId(int id) { id_ = id; }

std::chrono::time_point<std::chrono::system_clock> Launch::GetLaunchTime()
    const {
  return launch_time_;
}

std::chrono::time_point<std::chrono::system_clock> Launch::GetShutDownTime()
    const {
  return shutdown_time_;
}

static std::string FromTimepointToString(
    const std::chrono::time_point<std::chrono::system_clock> &tp) {
  std::stringstream ss;
  time_t t = std::chrono::system_clock::to_time_t(tp);
  ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

static std::chrono::time_point<std::chrono::system_clock> FromStrToTimepoint(
    const std::string &time_str) {
  std::tm date_time = {};
  std::istringstream iss(time_str);
  iss >> std::get_time(&date_time, "%Y-%m-%d %H:%M:%S");
  if (iss.fail()) {
    return {};
  }
  std::time_t t = std::mktime(&date_time);
  return std::chrono::system_clock::from_time_t(t);
}

std::string Launch::GetLaunchTimeStr() const {
  return FromTimepointToString(launch_time_);
}

std::string Launch::GetShutdownTimeStr() const {
  return FromTimepointToString(shutdown_time_);
}

void Launch::SetLaunchTime(
    std::chrono::time_point<std::chrono::system_clock> launch_time) {
  launch_time_ = launch_time;
}

void Launch::SetLaunchTime(const std::string &launch_time) {
  launch_time_ = FromStrToTimepoint(launch_time);
}

void Launch::SetShutdownTime(const std::string &shutdown_time) {
  shutdown_time_ = FromStrToTimepoint(shutdown_time);
}

void Launch::SetShutdownTime(
    std::chrono::time_point<std::chrono::system_clock> shutdown_time) {
  shutdown_time_ = shutdown_time;
}

int Launch::GetEngineId() const { return engine_id_; }

void Launch::SetEngineId(int engine_id) { engine_id_ = engine_id; }

std::string Launch::GetEngineName() const { return engine_name_; }

void Launch::SetEngineName(const std::string &name) { engine_name_ = name; }

Launch::Launch() {}

}  // namespace model
