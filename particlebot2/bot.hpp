#pragma once

#include <memory>

#include <particledi.hpp>

#include "ircsocket_base.hpp"
#include "event.hpp"

namespace pb2 {

  class bot_private;
  class ircsocket;
  class event;

  class bot : public particledi::dependency {
  public:
    typedef bot* ptr;
    
    bot(particledi::dm_ptr dm);
    virtual ~bot();

    void load_config();

    std::optional<ircsocket_base::ptr>
    get_socket(std::string& name);
     
    void emit(std::shared_ptr<event> e);
    
    void start();
    
    std::string get_prefix();
    
    particledi::dm_ptr get_dm();
    
    std::map<std::string, config_group_t> get_groups();
    
  private:
    std::shared_ptr<bot_private> priv;
  };
  
  typedef bot* bot_ptr;

}
