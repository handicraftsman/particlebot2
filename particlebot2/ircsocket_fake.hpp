#pragma once

#include <sstream>

#include "ircsocket_base.hpp"

namespace pb2 {

  class ircsocket_base;

  class ircsocket_fake : public ircsocket_base {
  public:
    ircsocket_fake();

    virtual ircstream stream();

    std::string get();
    void clear();

  protected:
    virtual void enqueue(std::string& msg);

  private:
    std::stringstream ss;
  };

}