#pragma once

#include <memory>

#include <particledi.hpp>

#include "ircsocket_base.hpp"

namespace pb2 {

  class bot_private;
  class ircsocket;

  class bot : public particledi::dependency {
  public:
    typedef bot* ptr;
    
    bot(particledi::dm_ptr dm);
    virtual ~bot();

    void load_config();

    std::optional<ircsocket_base::ptr>
    get_socket(std::string& name);

    void start();
    
  private:
    std::shared_ptr<bot_private> priv;
  };

}