#include "../particlebot2.hpp"

#include <sstream>

#include <sqlite3.h>

extern "C" {
  pb2::plugin* pb2_plugin;

  std::string get_calc(pb2::event_command::ptr e, std::string name) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "SELECT text FROM calcs WHERE server=:server AND channel=:channel AND name=:name LIMIT 1;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, namen;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, namen,    name.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      const char* _text = (const char*) sqlite3_column_text(stmt, 0);
      std::string text(_text);
      sqlite3_finalize(stmt);
      return text;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
    
    return "<no such calc>";
  }
  
  bool has_calc(pb2::event_command::ptr e, std::string name) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "SELECT server FROM calcs WHERE server=:server AND channel=:channel AND name=:name LIMIT 1;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, namen;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, namen,    name.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      sqlite3_finalize(stmt);
      return true;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
    
    return false;
  }
  
  void update_calc(pb2::event_command::ptr e, std::string name, std::string value) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "UPDATE calcs SET text=:text WHERE server=:server AND channel=:channel AND name=:name;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, namen, textn;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    textn    = sqlite3_bind_parameter_index(stmt, ":text");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, namen,    name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, textn,    value.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
  pb2c_update_calc_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2c_update_calc_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);
    }
    sqlite3_finalize(stmt);
  }
  
  void insert_calc(pb2::event_command::ptr e, std::string name, std::string value) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "INSERT INTO calcs (server, channel, name, text ) VALUES (:server, :channel, :name, :text);",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, namen, textn;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    textn    = sqlite3_bind_parameter_index(stmt, ":text");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, namen,    name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, textn,    value.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
  pb2c_insert_calc_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2c_insert_calc_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);
    }
    sqlite3_finalize(stmt);
  }
  
  void set_calc(pb2::event_command::ptr e, std::string name, std::string value) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    if (has_calc(e, name)) {
      update_calc(e, name, value);
    } else {
      insert_calc(e, name, value);
    }
  }
  
  void clear_calc(pb2::event_command::ptr e, std::string name) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "DELETE FROM calcs WHERE server=:server AND channel=:channel AND name=:name;",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, namen;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    namen    = sqlite3_bind_parameter_index(stmt, ":name");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, namen,    name.c_str(), -1, SQLITE_TRANSIENT);
    
    int ret;
  pb2c_clear_calc_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2c_clear_calc_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);   
    }
    sqlite3_finalize(stmt);
  }
  
  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {    
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    char* emsg = NULL;
    
    sqlite3_exec(
      db_s->get_db(),
      "CREATE TABLE IF NOT EXISTS calcs ("
        "server  VARCHAR(64) NOT NULL, "
        "channel VARCHAR(64), "
        "name    VARCHAR(64), "
        "text    VARCHAR(512) "
      ");",
      NULL,
      NULL,
      &emsg
    );
    if (emsg) {
      throw std::runtime_error(std::string(emsg));
    }
    
    pb2::command calc_cmd {
      .pplugin     = pb2_plugin,
      .name        = "calc",
      .usage       = "<name> [= <text...>]",
      .description = "shows and sets calcs",
      .cooldown    = 5,
      .flag        = "enable",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        if (e->split.size() == 2) {
          e->socket->stream() << pb2::ircstream::reply(e, e->split[1] + " = " + get_calc(e, e->split[1]));
        } else if (e->split.size() >= 4 && e->split[2] == "=") {
          if (e->split.size() == 4 && (e->split[3] == "null" || e->split[3] == "nullptr")) {
            clear_calc(e, e->split[1]);
            e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
          } else {
            std::stringstream ss;
            for (int i = 3; i < e->split.size(); ++i) {
              ss << e->split[i] << " ";
            }
            ss << "[" << e->nick << "]";
            set_calc(e, e->split[1], ss.str());
            e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
          }
        } else {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        }
      }
    };
    pb2_plugin->register_command(calc_cmd);
  }
  
  void pb2_deinit() {}
  
}
