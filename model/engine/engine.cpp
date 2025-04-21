#include "engine.h"

namespace model {

Engine::Engine(const Characteristics &ch) : characteristics_(ch) {}

Characteristics Engine::GetСharacteristics() const { return characteristics_; }

void Engine::SetСharacteristics(const Characteristics &ch) {
  characteristics_ = ch;
}

void Engine::SetId(int id) { id_ = id; }

int Engine::GetId() const { return id_; }

void Engine::SetName(const std::string &name) { name_ = name; }

std::string Engine::GetName() const { return name_; }
}  // namespace model
