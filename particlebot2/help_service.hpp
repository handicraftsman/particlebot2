#pragma once

#include <particledi.hpp>

namespace pb2 {
 
  class help_service : public particledi::dependency {
  public:
    help_service();
    virtual ~help_service();
  };

}