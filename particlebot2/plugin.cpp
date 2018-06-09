#include "plugin.hpp"

#include <future>
#include <thread>

#include <dlfcn.h>

static std::string find_path_to_plugin(std::string name) {
  const std::vector<std::string> paths {
    "./libpb2-" + name + ".so",
    "/usr/share/libpb2-" + name + ".so",
    "/usr/local/share/libpb2-" + name + ".so"
  };

  for (std::string path : paths)
    if (std::ifstream(path).good())
      return path;

  throw std::runtime_error("Unable to find plugin " + name);
}

typedef void (*init_func)(std::vector<std::pair<std::string, std::string>>& cfg);
typedef void (*deinit_func)();

namespace pb2 {

  plugin::plugin(std::string& name, std::vector<std::pair<std::string, std::string>>& cfg)
  : handle(nullptr)
  , l("?" + name)
  {
    std::string path = find_path_to_plugin(name);
    
    void* h = dlopen(path.c_str(), RTLD_LAZY);
    if (!h) {
      throw std::runtime_error(std::string(dlerror()));
    }
    dlerror();
    
    handle = std::shared_ptr<void>(h, dlclose);
    
    char* error= nullptr;
    
    plugin** p = (plugin**) dlsym(handle.get(), "pb2_plugin");
    if ((error = dlerror()) == nullptr) {
      *p = this;
    }
    
    init_func init_f = (init_func) dlsym(handle.get(), "pb2_init");
    if ((error = dlerror()) == nullptr) {
      init_f(cfg);
    }

    l.debug("Initialized a new plugin!");
  }

  plugin::~plugin() {
    char* error = nullptr;
    
    deinit_func deinit_f = (deinit_func) dlsym(handle.get(), "pb2_deinit");
    if ((error = dlerror()) == nullptr) {
      deinit_f();
    }
  }
  
  void plugin::emit(event::ptr e) {
    event* eptr = e.get();
    std::string id(typeid(*eptr).name());
    std::optional<event_handler> handler_opt = event_handlers[id];
    if (handler_opt) {
      event_handler handler = handler_opt.value();
      std::shared_ptr<void> _handle = handle;
      std::async([handler, e, _handle] () {
        handler(e);
      });
    }
  }

}