#pragma once

#include <particledi.hpp>

#include "ircsocket_base.hpp"

#include "ircstream.hpp"

#include "config.hpp"
namespace pb2 {
  
  class ircsocket_base;
  class ircsocket_private;
  class ircstream;

  class ircsocket : public ircsocket_base {
  public:
    friend ircsocket_private;
    friend ircstream;

    ircsocket(std::weak_ptr<particledi::dm> dm, std::string name, config_server_t& cfg);
    ircsocket();

    virtual ~ircsocket();

    virtual ircstream stream();

  protected:
    virtual void enqueue(std::string& msg);

  private:
    std::shared_ptr<ircsocket_private> priv;
  };

}