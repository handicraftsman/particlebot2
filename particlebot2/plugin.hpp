#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <tuple>
#include <typeinfo>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include <guosh.hpp>

#include <cxxabi.h>

#include "event.hpp"

namespace pb2 {
  class plugin_service;
  class plugin;
  class plugin_private;

  typedef std::function<void(event::ptr)> event_handler;

  struct command {
    typedef std::function<void(command& c, event_command::ptr e)> handler_t;
    typedef std::string server_name_t;
    typedef std::string channel_name_t;
    typedef std::string host_name_t;
    
    command();
    command(
      plugin*     pplugin,
      std::string name,
      std::string usage,
      std::string description,
      int         cooldown,
      std::string flag,
      handler_t   handler
    );
    
    void handle(event_command::ptr e);
        
    plugin* pplugin;
    std::string name;
    std::string usage;
    std::string description;
    int cooldown;
    std::string flag;
    handler_t handler;
    
    std::map<
      std::tuple<server_name_t, channel_name_t, host_name_t>,
      std::chrono::time_point<std::chrono::system_clock>
    > last_uses;
  };
  
  class plugin {
  public:
    typedef std::shared_ptr<plugin> ptr;
    
    friend plugin_service;
 
    plugin(std::string& name, std::vector<std::pair<std::string, std::string>>& cfg);
    ~plugin();

    template<class T>
    void register_event_handler(std::function<void(event::ptr)> func) {
      register_event_handler_<T>((event_handler) func);
    }
    
    template<class T>
    void register_event_handler_(event_handler func) {
      std::string name = std::string(typeid(T).name());
      event_handlers.emplace(name, func);
      char* n = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, nullptr);
      l.debug("Registered an event handler for %s", n);
      free(n);
    }
    
    void register_command(command& c);
    
    bool handle_command(event_command::ptr e);

    std::string name;

    Guosh::Logger l;

    std::map<std::string, std::optional<event_handler>> event_handlers;
    std::map<std::string, command> commands;
    
  protected:
    void emit(event::ptr e);

  private:
    std::shared_ptr<void> handle;
  };
}

#define pb2_ptrcast std::reinterpret_pointer_cast

extern "C" {
  extern pb2::plugin* pb2_plugin;
  
  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg);
  void pb2_deinit();
}
