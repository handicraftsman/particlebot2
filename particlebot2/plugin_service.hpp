#pragma once

#include <memory>
#include <optional>

#include <particledi.hpp>

#include "plugin.hpp"

namespace pb2 {

  class bot;
  class bot_private;
  
  class plugin_service : public particledi::dependency {
  public:
    friend bot;
    friend bot_private;
  
    typedef std::shared_ptr<plugin_service> ptr;
  
    plugin_service();
    virtual ~plugin_service();

    std::optional<plugin::ptr> get(std::string& name);

    void emit(event::ptr e);

  protected:
    void load_plugin(std::string& name, std::vector<std::pair<std::string, std::string>>& cfg);

  private:
    std::map<std::string, plugin::ptr> plugins;
  };
}