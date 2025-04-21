#pragma once
#include <chrono>
#include <string>
namespace model {

class Launch
{
private:
    int id_;
    std::chrono::time_point<std::chrono::system_clock> launch_time_;
    std::chrono::time_point<std::chrono::system_clock> shutdown_time_;
    int engine_id_;
    std::string engine_name_;


public:
    Launch();
    int GetId() const;
    void SetId(int id);
    std::chrono::time_point<std::chrono::system_clock> GetLaunchTime() const;
    std::chrono::time_point<std::chrono::system_clock> GetShutDownTime() const;
    std::string GetLaunchTimeStr() const;
    std::string GetShutdownTimeStr() const;
    void SetLaunchTime(std::chrono::time_point<std::chrono::system_clock> launch_time);
    void SetShutdownTime(std::chrono::time_point<std::chrono::system_clock> shutdown_time);
    void SetLaunchTime(const std::string& launch_time);
    void SetShutdownTime(const std::string& shutdown_time);
    int GetEngineId() const;
    void SetEngineId(int engine_id);

    std::string GetEngineName() const;
    void SetEngineName(const std::string& name);
};

} // namespace model
