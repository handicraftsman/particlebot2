#pragma once

#include <particledi.hpp>

#include "config.hpp"

namespace pb2 {
  
  class config_service : public particledi::dependency {
  public:
    typedef std::shared_ptr<config_service> ptr;
    
    config_service(std::string path);
    virtual ~config_service();
    config_t get_config();

  private:
    std::string path;
  };

}