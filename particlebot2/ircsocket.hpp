#pragma once

#include <particledi.hpp>

#include <guosh.hpp>

#include "ircsocket_base.hpp"

#include "ircstream.hpp"

#include "config.hpp"


namespace pb2 {
  
  class bot;
  class ircsocket_base;
  class ircsocket_private;
  class ircstream;

  class ircsocket : public ircsocket_base {
  public:
    friend bot;
    friend ircsocket_private;
    friend ircstream;

    ircsocket(std::weak_ptr<particledi::dm> dm, std::string name, config_server_t& cfg);
    ircsocket();

    virtual ~ircsocket();

    virtual ircstream stream();
    virtual void flushq();

    virtual void connect();

    virtual void autojoin();
    
    virtual std::string get_name();
    
    virtual config_server_t& get_config();
    
    virtual std::map<std::string, user>& get_user_cache();

  protected:
    virtual void enqueue(std::string& msg);

  private:
    std::string name;
    std::shared_ptr<ircsocket_private> priv;
  };

}
