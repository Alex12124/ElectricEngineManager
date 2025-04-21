#pragma once

#include <vector>
#include "engine.h"
#include <QSqlDatabase>


namespace model {

class EngineDao
{
public:
    EngineDao(const std::string& connection_name);
    ~EngineDao() = default;

    void Init() const;

    bool Add(Engine& engine) const;
    void Update(const Engine& engine) const;
    bool Remove(int id) const;
    bool Remove(std::vector<int> id) const;
    std::vector<Engine> Engines() const;

private:
    std::string connection_name_;
};
}
