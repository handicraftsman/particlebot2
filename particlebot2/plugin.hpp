#pragma once

#include <functional>
#include <typeinfo>
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include <guosh.hpp>

#include <cxxabi.h>

#include "event.hpp"

namespace pb2 {
  class plugin_service;
  class plugin_private;

  typedef std::function<void(event::ptr)> event_handler;

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

    std::string name;

    Guosh::Logger l;

  protected:
    void emit(event::ptr e);

  private:
    std::shared_ptr<void> handle;
    
    std::map<std::string, std::optional<event_handler>> event_handlers;
  };
}

#define pb2_ptrcast std::dynamic_pointer_cast

extern "C" {
  extern pb2::plugin* pb2_plugin;
  
  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg);
  void pb2_deinit();
}