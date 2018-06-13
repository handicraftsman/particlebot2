#include "plugin.hpp"

#include <future>
#include <thread>

#include <dlfcn.h>

#include "db_service.hpp"

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

  command::command()
  : pplugin(nullptr)
  , name("<unknown>")
  , usage("")
  , description("<none>")
  , cooldown(0)
  , flag("world")
  , handler([] (command& c, event_command::ptr e) {})
  {}
  
  command::command(
    plugin*     _pplugin,
    std::string _name,
    std::string _usage,
    std::string _description,
    int         _cooldown,
    std::string _flag,
    handler_t   _handler
  )
  : pplugin(_pplugin)
  , name(_name)
  , usage(_usage)
  , description(_description)
  , cooldown(_cooldown)
  , flag(_flag)
  , handler(_handler)
  {}
  
  void command::handle(event_command::ptr e) {
    if (pplugin == nullptr) {
      throw std::runtime_error("pplugin field is null in " + name);
    }
    
    std::tuple tpl { e->socket->get_name(), e->target, e->host };
    auto current = std::chrono::system_clock::now();
    
    if (cooldown != 0) {
      if (last_uses.find(tpl) != last_uses.end()) {
        auto last = last_uses[tpl];
        if ((current - last) < std::chrono::seconds(cooldown)) {
          return;
        }
      }
    }
    
    particledi::dm_ptr dm = e->pbot->get_dm();
    std::shared_ptr<db_service> db_s = dm->get<db_service>();
    
    pb2::flag f(e->socket->get_name());
    f.channel = e->target;
    f.host    = e->host;
    f.plugin  = pplugin->name;
    f.name    = flag;
    
    if (db_s->check(f)) {
      last_uses[tpl] = current;
      handler(*this, e);
    }
  }
  
  plugin::plugin(std::string& _name, std::vector<std::pair<std::string, std::string>>& cfg)
  : handle(nullptr)
  , name(_name)
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
      std::async([this, &id, handler, e, _handle] () {
        try {
          handler(e);
        } catch (std::exception& exc) {
          char* exc_name = abi::__cxa_demangle(typeid(exc).name(), nullptr, nullptr, nullptr);
          char* did = abi::__cxa_demangle(id.c_str(), nullptr, nullptr, nullptr);
          l.error("Exception: %s (%s)\n\tin %s handler", exc_name, exc.what(), did);
          free(did);
          free(exc_name);
        }
      });
    }
  }
  
  void plugin::register_command(command& c) {
    commands[c.name] = c;
    l.debug("Registered the %s command", c.name.c_str());
  }

  bool plugin::handle_command(event_command::ptr e) {
    if (commands.find(e->cmd) != commands.end()) {
      command& c = commands[e->cmd];
      
      c.handle(e);
      
      return true;
    }
    return false;
  }
  
}
