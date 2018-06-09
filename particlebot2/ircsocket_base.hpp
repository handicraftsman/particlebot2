#pragma once

#include <memory>

#include "ircstream.hpp"

namespace pb2 {

  class bot;
  class ircstream;

  class ircsocket_base {
  public:
    typedef std::shared_ptr<ircsocket_base> ptr;
    
    friend bot;
    friend ircstream;

    virtual ~ircsocket_base();

    virtual ircstream stream() = 0;

  protected:
    virtual void connect() = 0;

    virtual void enqueue(std::string& msg) = 0;
  };

}