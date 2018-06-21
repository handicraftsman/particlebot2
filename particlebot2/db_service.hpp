#pragma once

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include <particledi.hpp>

typedef struct sqlite3 sqlite3;

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
    std::vector<flag> list(flag& f);

    sqlite3* get_db();
    
  private:    
    std::shared_ptr<db_service_private> priv;
  };

}
