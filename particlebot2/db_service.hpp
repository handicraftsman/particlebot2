#pragma once

#include <particledi.hpp>

namespace pb2 {

  class db_service : public particledi::dependency {
  public:
    typedef std::shared_ptr<db_service> ptr;
    
    db_service(std::string path);
    virtual ~db_service();
  };

}