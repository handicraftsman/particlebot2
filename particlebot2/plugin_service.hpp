#pragma once

#include <particledi.hpp>

namespace pb2 {

  class plugin_service : public particledi::dependency {
  public:
    plugin_service();
    virtual ~plugin_service();
  };

}