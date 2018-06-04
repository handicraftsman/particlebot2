#pragma once

#include <particledi.hpp>

#include "ircstream.hpp"

namespace pb2 {
  
  class ircsocket_private;
  class ircstream;

  class ircsocket {
  public:
    friend ircsocket_private;
    friend ircstream;

    ircsocket(particledi::dm_ptr dm);

    ircstream stream();

  protected:
    void enqueue(std::string& msg);

  private:
    std::shared_ptr<ircsocket_private> priv;
  };

}