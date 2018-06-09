#pragma once

#include <memory>

#include "bot.hpp"

namespace pb2 {

  class event {
  public:
    typedef std::shared_ptr<event> ptr;
    
    event();
    virtual ~event();
  };
  
  class event_test : event {
  private:
    event_test(bot::ptr bot);
  
  public:
    typedef std::shared_ptr<event_test> ptr;
    
    static event::ptr create(bot::ptr bot);
    virtual ~event_test();
    
    bot::ptr pbot;
  
  };

}