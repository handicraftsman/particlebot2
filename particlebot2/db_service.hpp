#pragma once

#include <particledi.hpp>

namespace pb2 {

  class db_service : public particledi::dependency {
  public:
    db_service(std::string path);
    virtual ~db_service();
  };

}