#pragma once

#include <memory>

#include "ircstream.hpp"

namespace pb2 {

  class bot;
  class bot_private;
  class ircstream;

  class ircsocket_base {
  public:
    typedef std::shared_ptr<ircsocket_base> ptr;
    typedef ircsocket_base* cptr;
    
    friend bot;
    friend bot_private;
    friend ircstream;

    virtual ~ircsocket_base();

    virtual ircstream stream() = 0;
    virtual void flushq() = 0;

    virtual void connect() = 0;

  protected:
    virtual void enqueue(std::string& msg) = 0;
  };

}