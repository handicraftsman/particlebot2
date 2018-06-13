#pragma once

#include <optional>
#include <memory>
#include <string>

#include <particledi.hpp>

namespace pb2 {
  
  struct flag {
    flag(std::string server);
    flag() = delete;

    std::string server;
    std::optional<std::string> channel;
    std::optional<std::string> host;
    std::optional<std::string> plugin;
    std::optional<std::string> name;
  };

  class db_service_private;
  class db_service : public particledi::dependency {
  public:
    typedef std::shared_ptr<db_service> ptr;

    db_service(std::string path);
    virtual ~db_service();

    bool check(flag& f);
    void insert(flag& f);
    void remove(flag& f);

  private:    
    std::shared_ptr<db_service_private> priv;
  };

}
