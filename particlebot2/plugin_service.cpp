#include "plugin_service.hpp"

#include <algorithm>

namespace pb2 {

  plugin_service::plugin_service() {}
  plugin_service::~plugin_service() {}

  std::optional<plugin::ptr> plugin_service::get(std::string& name) {
    if (std::find_if(plugins.begin(), plugins.end(), [&] (std::pair<std::string, plugin::ptr> p) {
      return p.first == name;
    }) != plugins.end()) {
      return plugins[name];
    } else {
      return std::nullopt;
    }
  }

  void plugin_service::load_plugin(std::string& name, std::vector<std::pair<std::string, std::string>>& cfg) {
    plugin::ptr p(new plugin(name, cfg));

    plugins[name] = p;
  }
  
  void plugin_service::emit(event::ptr e) {
    for (std::pair<std::string, plugin::ptr> p : plugins) {
      p.second->emit(e);
    }
  }
  
  void plugin_service::handle_command(event_command::ptr e) {
    for (std::pair<std::string, plugin::ptr> p : plugins) {
      p.second->handle_command(e);
    }
  }

}
