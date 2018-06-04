#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "ircsocket.hpp"

namespace pb2 {

  class ircsocket;

  class ircstream : public std::ostream, std::streambuf {
  public:
    ircstream(ircsocket& socket);
    virtual ~ircstream();
  
  protected:
    std::streambuf::int_type overflow(std::streambuf::int_type c);
  
  private:
    ircsocket& socket;
    std::stringstream buf;
    int prev;
  };

}