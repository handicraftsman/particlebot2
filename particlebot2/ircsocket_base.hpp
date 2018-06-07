#pragma once

#include "ircstream.hpp"

namespace pb2 {

  class ircstream;

  class ircsocket_base {
  public:
    friend ircstream;

    virtual ~ircsocket_base();

    virtual ircstream stream() = 0;

  protected:
    virtual void enqueue(std::string& msg) = 0;
  };

}