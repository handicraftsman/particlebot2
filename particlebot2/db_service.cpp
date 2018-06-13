#include "db_service.hpp"

#include <sstream>

#include <guosh.hpp>
#include <sqlite3.h>

namespace pb2 {

  /*
   * Flag implementation
   */

  flag::flag(std::string _server)
  : server(_server)
  {}
  
  /*
   * Private declaration
   */
  
  class db_service_private {
  public:
    db_service_private(db_service& pub, std::string& path);
    
    bool check(flag& f);
    void insert(flag& f);
    void remove(flag& f);
    
    db_service& pub;
    
    Guosh::Logger l;
    
    sqlite3* db;
  };
  
  /*
   * Private implementation
   */
  
  db_service_private::db_service_private(db_service& _pub, std::string& path)
  : pub(_pub)
  , l("db")
  , db(nullptr)
  {
    sqlite3_open(path.c_str(), &db);
    if (!db) {
      const char* emsg = sqlite3_errmsg(db);
      throw std::runtime_error(std::string(emsg));
    }
    
    char* emsg = NULL;
  
    sqlite3_exec(
      db, 
      "CREATE TABLE IF NOT EXISTS flags ("
        "server  VARCHAR(64) NOT NULL, "
        "channel VARCHAR(64), "
        "host    VARCHAR(64), "
        "plugin  VARCHAR(64), "
        "name    VARCHAR(64) "
      ");",
      NULL,
      NULL,
      &emsg
    );
    if (emsg) {
      throw std::runtime_error(std::string(emsg));
    }
    
    l.debug("Initialized pb2::db_service and pb2::db_service_private");
  }
  
  bool db_service_private::check(flag& f) {
    if (f.name == "world") return true;
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db,
      "SELECT "
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND host=? AND plugin=? AND name=? LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND plugin=? AND name=? AND host IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND host=? AND plugin=? AND name=? AND channel IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND plugin=? AND name=? AND channel IS NULL AND host IS NULL LIMIT 1) "
      "OR "
    
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND host=? AND plugin=? AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND plugin=? AND host IS NULL AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND host=? AND plugin=? AND channel IS NULL AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND plugin=? AND channel IS NULL AND host IS NULL AND name IS NULL LIMIT 1) "
      "OR "
    
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND host=? AND plugin IS NULL AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel=? AND host IS NULL AND plugin IS NULL AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND host=? AND channel IS NULL AND plugin IS NULL AND name IS NULL LIMIT 1) "
      "OR "
        "EXISTS(SELECT name FROM flags WHERE server=? AND channel IS NULL AND host IS NULL AND plugin IS NULL AND name IS NULL LIMIT 1);"
      ,
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db)));
      return false;
    }
    
    const char* server  = f.server.c_str();
    const char* channel = f.channel ? f.channel.value().c_str() : NULL;
    const char* host    = f.host    ? f.host.value().c_str()    : NULL;
    const char* plugin  = f.plugin  ? f.plugin.value().c_str()  : NULL;
    const char* name    = f.name    ? f.name.value().c_str()    : NULL;
    
    std::cout << server << " " << channel << " " << host << " " << plugin << " " << name << std::endl;
    
    sqlite3_bind_text(stmt, 1, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, channel, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, host, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, plugin, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, name, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 6, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, channel, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, plugin, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, name, -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 10, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, host, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, plugin, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 13, name, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 14, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 15, plugin, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 16, name, -1, SQLITE_TRANSIENT);
    
    
    sqlite3_bind_text(stmt, 17, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 18, channel, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 19, host, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 20, plugin, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 21, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 22, channel, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 23, plugin, -1, SQLITE_TRANSIENT);
      
    sqlite3_bind_text(stmt, 24, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 25, host, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 26, plugin, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 27, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 28, plugin, -1, SQLITE_TRANSIENT);


    sqlite3_bind_text(stmt, 29, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 30, channel, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 31, host, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 32, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 33, channel, -1, SQLITE_TRANSIENT);
      
    sqlite3_bind_text(stmt, 34, server, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 35, host, -1, SQLITE_TRANSIENT);
    
    sqlite3_bind_text(stmt, 36, server, -1, SQLITE_TRANSIENT);
    
    int r = sqlite3_step(stmt);
    if (r == SQLITE_ROW) {
      int out = sqlite3_column_int(stmt, 0);
      sqlite3_finalize(stmt);
      std::cout << out << std::endl;
      return (bool) out;
    } else {
      sqlite3_finalize(stmt);
      std::cout << "ugh... false" << std::endl;
      return false;
    }
  }
  
  void db_service_private::insert(flag& f) {
    std::stringstream ss;
    
    ss << "INSERT INTO flags(server";
    if (f.channel) ss << ",channel";
    if (f.host)    ss << ",host";
    if (f.plugin)  ss << ",plugin";
    if (f.name)    ss << ",name";
    ss << ") VALUES (" << f.server;
    if (f.channel) ss << ",:server";
    if (f.host)    ss << ",:host";
    if (f.plugin)  ss << ",:plugin";
    if (f.name)    ss << ",:name";
    ss << ");";
    
    std::string req = ss.str();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db,
      req.c_str(),
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db)));
    }
    
    int servern, channeln, hostn, pluginn, namen;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    hostn    = sqlite3_bind_parameter_index(stmt, ":host");
    pluginn  = sqlite3_bind_parameter_index(stmt, ":plugin");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    
    if (servern)               sqlite3_bind_text(stmt, servern, f.server.c_str(), -1, SQLITE_TRANSIENT);
    if (channeln && f.channel) sqlite3_bind_text(stmt, channeln, f.channel.value().c_str(), -1, SQLITE_TRANSIENT);
    if (hostn    && f.host)    sqlite3_bind_text(stmt, hostn, f.host.value().c_str(), -1, SQLITE_TRANSIENT);
    if (pluginn  && f.plugin)  sqlite3_bind_text(stmt, pluginn, f.plugin.value().c_str(), -1, SQLITE_TRANSIENT);
    if (namen    && f.name)    sqlite3_bind_text(stmt, namen, f.name.value().c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
  pb2_flag_insert_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2_flag_insert_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db)));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
  }
  
  void db_service_private::remove(flag& f) {
    std::stringstream ss;
    
    ss << "DELETE FROM flags WHERE server=:server";
    if (f.channel) ss << "AND channel=:channel";
    if (f.host)    ss << "AND host=:host";
    if (f.plugin)  ss << "AND plugin=:plugin";
    if (f.name)    ss << "AND name=:name";
    ss << ";";
    
    std::string req = ss.str();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db,
      req.c_str(),
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db)));
    }
    
    int servern, channeln, hostn, pluginn, namen;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    hostn    = sqlite3_bind_parameter_index(stmt, ":host");
    pluginn  = sqlite3_bind_parameter_index(stmt, ":plugin");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    
    if (servern)               sqlite3_bind_text(stmt, servern, f.server.c_str(), -1, SQLITE_TRANSIENT);
    if (channeln && f.channel) sqlite3_bind_text(stmt, channeln, f.channel.value().c_str(), -1, SQLITE_TRANSIENT);
    if (hostn    && f.host)    sqlite3_bind_text(stmt, hostn, f.host.value().c_str(), -1, SQLITE_TRANSIENT);
    if (pluginn  && f.plugin)  sqlite3_bind_text(stmt, pluginn, f.plugin.value().c_str(), -1, SQLITE_TRANSIENT);
    if (namen    && f.name)    sqlite3_bind_text(stmt, namen, f.name.value().c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
  pb2_flag_remove_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2_flag_remove_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db)));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
  }
  
  /*
   * Public implementation
   */

  db_service::db_service(std::string path)
  : priv(new db_service_private(*this, path))
  {}
  
  
  db_service::~db_service() {}

  bool db_service::check(flag& f) {
    return priv->check(f);
  }
  
  void db_service::insert(flag& f) {
    priv->insert(f);
  }
  
  void db_service::remove(flag& f) {
    priv->remove(f);
  }

}
