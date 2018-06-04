#pragma once

#include <particledi.hpp>

#include "config.hpp"

namespace pb2 {
  
  class config_service : public particledi::dependency {
  public:
    config_service(std::string path);
    virtual ~config_service();
    config_t get_config();

  private:
    std::string path;
  };

}