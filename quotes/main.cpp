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
      const char* _text = (const char*) sqlite3_column_text(stmt, 1);
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
      const char* _text = (const char*) sqlite3_column_text(stmt, 0);
      std::string text(_text);
      sqlite3_finalize(stmt);
      return std::to_string(id) + "] " + text;
    }
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);      
    }
    sqlite3_finalize(stmt);
    
    return "<no such quote>";
  }
  
  void insert_quote(pb2::event_command::ptr e, std::string quote) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
    
    int id = get_next_id(e);
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "INSERT INTO quotes (server, channel, text, id) VALUES (:server, :channel, :text, :id);",
      -1,
      &stmt,
      NULL
    ) != SQLITE_OK) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
    }
    
    int servern, channeln, textn, idn;
    servern  = sqlite3_bind_parameter_index(stmt, ":server");
    channeln = sqlite3_bind_parameter_index(stmt, ":channel");
    textn    = sqlite3_bind_parameter_index(stmt, ":text");
    idn      = sqlite3_bind_parameter_index(stmt, ":id");
    
    std::string sname = e->socket->get_name();
    
    sqlite3_bind_text(stmt, servern,  sname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, channeln, e->target.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, textn,    quote.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,  idn,      id);
    
    int ret;
  pb2c_insert_quote_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2c_insert_quote_step;
    if (ret == SQLITE_ERROR) {
      throw std::runtime_error(std::string(sqlite3_errmsg(db_s->get_db())));
      sqlite3_finalize(stmt);   
    }
    sqlite3_finalize(stmt);
  }
  
  void delete_quote(pb2::event_command::ptr e, int id) {
    pb2::db_service::ptr db_s = pb2_plugin->dm->get<pb2::db_service>();
            
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(
      db_s->get_db(),
      "DELETE FROM quotes WHERE server=:server AND channel=:channel AND id=:id;",
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
    sqlite3_bind_int(stmt, idn,       id);
    
    int ret;
  pb2c_delete_quote_step:
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) goto pb2c_delete_quote_step;
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
    
    pb2::command quote_cmd {
      .pplugin     = pb2_plugin,
      .name        = "quote",
      .usage       = "[id]",
      .description = "shows random or given quote",
      .cooldown    = 5,
      .flag        = "quote",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        if (e->split.size() == 1) {
          std::string quote = get_random_quote(e);
          e->socket->stream() << pb2::ircstream::reply(e, quote);
        } else if (e->split.size() == 2) {
          int id = std::stoi(e->split[1]);
          std::string quote = get_quote(e, id);
          e->socket->stream() << pb2::ircstream::reply(e, quote);
        } else {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        }
      }
    };
    pb2_plugin->register_command(quote_cmd);
    
    pb2::command add_quote_cmd {
      .pplugin     = pb2_plugin,
      .name        = "add-quote",
      .usage       = "<text...>",
      .description = "adds given quote to the database",
      .cooldown    = 0,
      .flag        = "add-quote",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        if (e->split.size() < 2) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        std::stringstream ss;
        for (int i = 1; i < e->split.size(); ++i) {
          ss << e->split[i] << " ";
        }
        ss << "[" << e->nick << "]";
        
        insert_quote(e, ss.str());
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      }
    };
    pb2_plugin->register_command(add_quote_cmd);
    
    pb2::command del_quote_cmd {
      .pplugin     = pb2_plugin,
      .name        = "del-quote",
      .usage       = "[id]",
      .description = "deletes given quote from the database",
      .cooldown    = 0,
      .flag        = "del-quote",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        if (e->split.size() != 2) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        int id = std::stoi(e->split[1]);
        delete_quote(e, id);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      }
    };
    pb2_plugin->register_command(del_quote_cmd);
  }
  
  void pb2_deinit() {}
}
