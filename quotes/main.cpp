#include "../particlebot2.hpp"

#include <sqlite3.h>

extern "C" {
  pb2::plugin* pb2_plugin;
  
  int get_next_id(pb2::event_command::ptr e) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "SELECT id FROM quotes WHERE server=:server AND channel=:channel ORDER BY id DESC LIMIT 1;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      int id = sqlite3_column_int(stmt, 0) + 1;
      sqlite3_finalize(stmt);
      return id;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
    
    return 0;
  }
  
  std::string get_random_quote(pb2::event_command::ptr e) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();

    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "SELECT id, text FROM quotes WHERE server=:server AND channel=:channel ORDER BY RANDOM() LIMIT 1;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      int id = sqlite3_column_int(stmt, 0);
      const unsigned char* _text = sqlite3_column_text(stmt, 1);
      std::string text(_text);
      sqlite3_finalize(stmt);
      return std::to_string(id) + "] " + text;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
    
    return "<no quotes available>";
  }
  
  std::string get_quote(pb2::event_command::ptr e, int id) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "SELECT text FROM quotes WHERE server=:server AND channel=:channel AND id=:id LIMIT 1;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, idn;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    idn      = sqlite3_bind_parameter_index(stmt, ":id");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, idn, id);
    
    int ret;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      const unsigned char* _text = sqlite3_column_text(stmt, 0);
      std::string text(_text);
      sqlite3_finalize(stmt);
      return std::to_string(id) + "] " + text;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt)
    
    return "<no such quote>";
  }
  
  void insert_quote(pb2::event_command::ptr e, std::string quote) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    // INSERT INTO quotes (server, channel, text, id) VALUES (:server, :channel, :text, :id);
  }
  
  void delete_quote(pb2::event_command::ptr e, int id) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    // DELETE FROM quotes WHERE server=:server AND channel=:channel AND id=:id;
  }
  
  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    char* emsg = NULL;
    
    sqlite3_exec(
      db_s->get_db(),
      "CREATE TABLE IF NOT EXISTS quotes ("
        "server  VARCHAR(64) NOT NULL, "
        "channel VARCHAR(64), "
        "text    VARCHAR(64), "
        "id      NUMBER NOT NULL PRIMARY KEY "
      ");",
      NULL,
      NULL,
      &emsg
    );
    if (emsg) {
      throw std::runtime_error(std::string(emsg));
    }
  }
  
  void pb2_deinit() {}
}
