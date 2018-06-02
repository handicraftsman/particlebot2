#pragma once

#include <particledi.hpp>

namespace pb2 {
  
  class config_service : public particledi::dependency {
  public:
    config_service(std::string path);
    virtual ~config_service();

  private:
    std::string path;
  };

}