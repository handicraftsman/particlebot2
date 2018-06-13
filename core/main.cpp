#include "../particlebot2.hpp"

#include <functional>
#include <regex>
#include <sstream>
#include <thread>

template<typename Out>
static void split_str(const std::string& s, char delim, Out result) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

static std::vector<std::string> split_str(const std::string& s, char delim) {
  std::vector<std::string> e;
  split_str(s, delim, std::back_inserter(e));
  return e;
}

static std::string join_strings(std::vector<std::string> v, char delim = ' ', int start = 0) {
  std::stringstream ss;
  int s = v.size();
  for (int i = start; i < s; ++i) {
    ss << v[i];
    if (i != (s - 1)) {
      ss << delim;
    }
  }
  return ss.str();
}

using namespace std::regex_constants;

static std::regex regex_code("^:.+? (\\d\\d\\d) .+? (.+)$", ECMAScript | icase);
static std::regex regex_ping("^PING :(.+)$", ECMAScript | icase);
static std::regex regex_join("^:(.+?)!(.+?)@(.+?) JOIN (.+)$", ECMAScript | icase);
static std::regex regex_part("^:(.+?)!(.+?)@(.+?) PART (.+?) :(.+)$", ECMAScript | icase);
static std::regex regex_privmsg("^:(.+?)!(.+?)@(.+?) PRIVMSG (.+?) :(.+)$", ECMAScript | icase);
static std::regex regex_notice("^:(.+?)!(.+?)@(.+?) NOTICE (.+?) :(.+)$", ECMAScript | icase);
static std::regex regex_nick("^:(.+?)!(.+?)@(.+?) NICK :(.+)$", ECMAScript | icase);

extern "C" {

  pb2::plugin* pb2_plugin;

  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    pb2_plugin->l.debug("Hello, World!");
    
    pb2_plugin->register_event_handler<pb2::event_test>([] (pb2::event::ptr _e) { 
      pb2::event_test::ptr e = pb2_ptrcast<pb2::event_test>(_e);
      pb2_plugin->l.debug("Received a test event");
    });
    
    pb2_plugin->register_event_handler<pb2::event_disconnect>([] (pb2::event::ptr _e) {
      pb2::event_disconnect::ptr e = pb2_ptrcast<pb2::event_disconnect>(_e);
      std::this_thread::sleep_for(std::chrono::seconds(5));
      std::thread(std::bind(&pb2::ircsocket_base::connect, e->socket)).detach();
    });
    
    pb2_plugin->register_event_handler<pb2::event_message>([] (pb2::event::ptr _e) {
      pb2::event_message::ptr e = pb2_ptrcast<pb2::event_message>(_e);
      
      auto m = std::smatch {};
      
      if ((m = std::smatch{}, std::regex_match(e->message, m, regex_privmsg))) {
        std::string nick = m[1], user = m[2], host = m[3], target = m[4], message = m[5];
        e->pbot->emit(pb2::event_privmsg::create(e->pbot, e->socket, nick, user, host, target, message));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_notice))) {
        std::string nick = m[1], user = m[2], host = m[3], target = m[4], message = m[5];
        e->pbot->emit(pb2::event_notice::create(e->pbot, e->socket, nick, user, host, target, message));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_code))) {
        int code = std::stoi(m[1]); std::string extra = m[2];
        e->pbot->emit(pb2::event_code::create(e->pbot, e->socket, code, extra));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_ping))) {
        std::string target = m[1];
        e->pbot->emit(pb2::event_ping::create(e->pbot, e->socket, target));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_join))) {
        std::string nick = m[1], user = m[2], host = m[3], channel = m[4];
        e->pbot->emit(pb2::event_join::create(e->pbot, e->socket, nick, user, host, channel));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_part))) {
        std::string nick = m[1], user = m[2], host = m[3], channel = m[4], reason = m[5];
        e->pbot->emit(pb2::event_part::create(e->pbot, e->socket, nick, user, host, channel, reason));
      } else if ((m = std::smatch{}, std::regex_match(e->message, m, regex_nick))) {
        std::string nick = m[1], user = m[2], host = m[3], new_nick = m[4];
        e->pbot->emit(pb2::event_nick::create(e->pbot, e->socket, nick, user, host, new_nick));
      }
    });
    
    pb2_plugin->register_event_handler<pb2::event_ping>([] (pb2::event::ptr _e) {
      pb2::event_ping::ptr e = pb2_ptrcast<pb2::event_ping>(_e);
      e->socket->stream() << ("PONG " + e->target + "\r\n");
    });
    
    pb2_plugin->register_event_handler<pb2::event_code>([] (pb2::event::ptr _e) {
      pb2::event_code::ptr e = pb2_ptrcast<pb2::event_code>(_e);
      if (e->code == 1) {
        e->socket->autojoin();
      }
    });
    
    pb2_plugin->register_event_handler<pb2::event_privmsg>([] (pb2::event::ptr _e) {
      pb2::event_privmsg::ptr e = pb2_ptrcast<pb2::event_privmsg>(_e);
    
      std::string prefix = e->pbot->get_prefix();
      if (e->message.size() > prefix.size() && e->message.substr(0, prefix.size()) == prefix) {
        std::string message = e->message.substr(prefix.size(), e->message.size()-1);
        std::vector<std::string> split = split_str(message, ' ');
        std::string cmd = split[0];
        
        e->pbot->emit(pb2::event_command::create(e->pbot, e->socket, e->nick, e->user, e->host, e->target, cmd, split));
      } else if (e->message[0] == '\x01' && e->message[e->message.size()-1] == '\x01') {
        std::string message = e->message.substr(1, e->message.size()-2);
        std::vector<std::string> split = split_str(message, ' ');
        std::string type = split[0];
        std::string data = join_strings(split, ' ', 1);
        
        e->pbot->emit(pb2::event_ctcp::create(e->pbot, e->socket, e->nick, e->user, e->host, e->target, type, data));
      }
    });
    
    pb2_plugin->register_event_handler<pb2::event_ctcp>([] (pb2::event::ptr _e) {
      pb2::event_ctcp::ptr e = pb2_ptrcast<pb2::event_ctcp>(_e);
      if (e->type == "VERSION") {
        e->socket->stream() << pb2::ircstream::nctcp(e->nick, "VERSION", "ParticleBot2 - An IRC bot in C++ - https://github.com/handicraftsman/particlebot2/");
      }
    });
    
    pb2_plugin->register_event_handler<pb2::event_notice>([] (pb2::event::ptr _e) {
      pb2::event_notice::ptr e = pb2_ptrcast<pb2::event_notice>(_e);
      
      if (e->message[0] == '\x01' && e->message[e->message.size()-1] == '\x01') {
        std::string message = e->message.substr(1, e->message.size()-2);
        std::vector<std::string> split = split_str(message, ' ');
        std::string type = split[0];
        std::string data = join_strings(split, ' ', 1);
        
        e->pbot->emit(pb2::event_nctcp::create(e->pbot, e->socket, e->nick, e->user, e->host, e->target, type, data));
      }
    });
    
    pb2_plugin->register_event_handler<pb2::event_command>([] (pb2::event::ptr _e) {
      pb2::event_command::ptr e = pb2_ptrcast<pb2::event_command>(_e);
      
      particledi::dm_ptr dm = e->pbot->get_dm();
      std::shared_ptr<pb2::plugin_service> pl_s = dm->get<pb2::plugin_service>();
      
      pl_s->handle_command(e);
    });
    
    pb2::command cmd_ping {
      .pplugin     = pb2_plugin,
      .name        = "ping",
      .usage       = "",
      .description = "Ping!",
      .cooldown    = 10,
      .flag        = "world",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        e->socket->stream() << pb2::ircstream::reply(e, "pong");
      }
    };
    pb2_plugin->register_command(cmd_ping);
    
    pb2::command cmd_key {
      .pplugin     = pb2_plugin,
      .name        = "key",
      .usage       = "| <key>",
      .description = "Generates a key when no arguments are provided. Gives you admin permissions when you type correct key",
      .cooldown    = 0,
      .flag        = "world",
      .handler     = [pb2_plugin] (pb2::command& c, pb2::event_command::ptr e) {
        static char* key = nullptr;
        
        if (e->split.size() == 1) {
          const int sz = 64;
          if (key != nullptr) delete[] key;
          key = new char[sz + 1];
                    
          static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
          ;
          
          srand(time(NULL));
          for (int i = 0; i < sz; ++i) {
            key[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
          }

          key[sz] = '\0';
          
          pb2_plugin->l.debug("Your key: %s", key);
          e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
        } else if (e->split.size() == 2) {
          if (key == nullptr || (std::string(key) != e->split[1])) {
            e->socket->stream() << pb2::ircstream::nreply(e, "Invalid key!");
          } else {
            particledi::dm_ptr dm = e->pbot->get_dm();
            std::shared_ptr<pb2::db_service> db_s = dm->get<pb2::db_service>();
            
            pb2::flag f(e->socket->get_name());
            f.host    = e->host;
            
            db_s->insert(f);
            
            e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
          }
          delete[] key;
          key = nullptr;
        } else {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        }
      }
    };
    pb2_plugin->register_command(cmd_key);
    
    auto flag_handler = [] (pb2::command& c, pb2::event_command::ptr e) {
      particledi::dm_ptr dm = e->pbot->get_dm();
      std::shared_ptr<pb2::db_service> db_s = dm->get<pb2::db_service>();
      
      if (e->cmd == "flag-list") {
        if (e->split.size() != 6) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        
        pb2::flag f(e->split[1]);
        f.channel = e->split[2] == "*" ? std::nullopt : std::optional(e->split[2]);
        f.host    = e->split[3] == "*" ? std::nullopt : std::optional(e->split[3]);
        f.plugin  = e->split[4] == "*" ? std::nullopt : std::optional(e->split[4]);
        f.name    = e->split[5] == "*" ? std::nullopt : std::optional(e->split[5]);
        
        std::vector<pb2::flag> flags = db_s->list(f);
        
        pb2::ircstream s = e->socket->stream();
        for (pb2::flag f : flags) {
          s << pb2::ircstream::nreply(
            e,
            "> server=[" + f.server +
            "] channel=[" + f.channel.value_or("<*>") +
            "] host=[" + f.host.value_or("<*>") +
            "] plugin=[" + f.plugin.value_or("<*>") +
            "] name=[" + f.name.value_or("<*>") +
            "]"
          );
        }
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-insert") {
        if (e->split.size() != 6) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        
        pb2::flag f(e->split[1]);
        f.channel = e->split[2] == "*" ? std::nullopt : std::optional(e->split[2]);
        f.host    = e->split[3] == "*" ? std::nullopt : std::optional(e->split[3]);
        f.plugin  = e->split[4] == "*" ? std::nullopt : std::optional(e->split[4]);
        f.name    = e->split[5] == "*" ? std::nullopt : std::optional(e->split[5]);
        
        db_s->insert(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-remove") {
        if (e->split.size() != 6) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        
        pb2::flag f(e->split[1]);
        f.channel = e->split[2] == "*" ? std::nullopt : std::optional(e->split[2]);
        f.host    = e->split[3] == "*" ? std::nullopt : std::optional(e->split[3]);
        f.plugin  = e->split[4] == "*" ? std::nullopt : std::optional(e->split[4]);
        f.name    = e->split[5] == "*" ? std::nullopt : std::optional(e->split[5]);
        
        db_s->remove(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-givec") {
        if (e->split.size() != 4) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        pb2::flag f(e->socket->get_name());
        f.channel = e->split[1];
        f.plugin  = e->split[2];
        f.name    = e->split[3];
        db_s->insert(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-takec") {
        if (e->split.size() != 4) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        pb2::flag f(e->socket->get_name());
        f.channel = e->split[1];
        f.plugin  = e->split[2];
        f.name    = e->split[3];
        db_s->remove(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-giveh") {
        if (e->split.size() != 4) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        pb2::flag f(e->socket->get_name());
        f.host   = e->split[1];
        f.plugin = e->split[2];
        f.name   = e->split[3];
        db_s->insert(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      } else if (e->cmd == "flag-takeh") {
        if (e->split.size() != 4) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        pb2::flag f(e->socket->get_name());
        f.host   = e->split[1];
        f.plugin = e->split[2];
        f.name   = e->split[3];
        db_s->remove(f);
        e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
      }
    };
    
    pb2::command cmd_flag_list {
      .pplugin     = pb2_plugin,
      .name        = "flag-list",
      .usage       = "<server> <channel|*> <host|*> <plugin|*> <name|*>",
      .description = "lists flags",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_list);
    
    pb2::command cmd_flag_insert {
      .pplugin     = pb2_plugin,
      .name        = "flag-insert",
      .usage       = "<server> <channel|*> <host|*> <plugin|*> <name|*>",
      .description = "inserts given flag into the database",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_insert);
    
    pb2::command cmd_flag_remove {
      .pplugin     = pb2_plugin,
      .name        = "flag-remove",
      .usage       = "<server> <channel|*> <host|*> <plugin|*> <name|*>",
      .description = "removes given flag from the database",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_remove);
    
    pb2::command cmd_flag_givec {
      .pplugin     = pb2_plugin,
      .name        = "flag-givec",
      .usage       = "<channel> <plugin> <name>",
      .description = "gives given flag to the given channel",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_givec);
    
    pb2::command cmd_flag_takec {
      .pplugin     = pb2_plugin,
      .name        = "flag-takec",
      .usage       = "<channel> <plugin> <name>",
      .description = "takes given flag from the given channel",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_takec);
    
    pb2::command cmd_flag_giveh {
      .pplugin     = pb2_plugin,
      .name        = "flag-giveh",
      .usage       = "<host> <plugin> <name>",
      .description = "gives given flag to the given hostname",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_giveh);
    
    pb2::command cmd_flag_takeh {
      .pplugin     = pb2_plugin,
      .name        = "flag-takeh",
      .usage       = "<host> <plugin> <name>",
      .description = "takes given flag from the given hostname",
      .cooldown    = 0,
      .flag        = "admin",
      .handler     = flag_handler
    };
    pb2_plugin->register_command(cmd_flag_takeh);
    
    pb2::command cmd_help {
      .pplugin = pb2_plugin,
      .name        = "help",
      .usage       = "| <command>",
      .description = "shows help messages for commands",
      .cooldown    = 0,
      .flag        = "world",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        particledi::dm_ptr dm = e->pbot->get_dm();
        std::shared_ptr<pb2::plugin_service> pl_s = dm->get<pb2::plugin_service>();
        
        if (e->split.size() == 1) {
          e->socket->stream() << pb2::ircstream::nreply(
            e,
            "Type &B" + e->pbot->get_prefix() + "help&N to show this message. " +
            "Type &B" + e->pbot->get_prefix() + "help <command>&N to show info about a command. " +
            "Type &B" + e->pbot->get_prefix() + "list&N to list all plugins. " +
            "Type &B" + e->pbot->get_prefix() + "list <plugin>&N to list all commands in a plugin. "
          );
        } else if (e->split.size() == 2) {
          for (std::pair<std::string, pb2::plugin::ptr> p : pl_s->plugins) {
            if (p.second->commands.find(e->split[1]) != p.second->commands.end()) {
              pb2::command& c = p.second->commands[e->split[1]];
              e->socket->stream() << pb2::ircstream::nreply(
                e,
                "> " +
                p.first + "/" + c.name + " | " +
                c.name + " " + c.usage + " | " +
                c.description + " | cooldown=" +
                std::to_string(c.cooldown) + " | flag=[" +
                p.first + "/" + c.flag + "]"
              );
            }
          }
        } else {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        }
      }
    };
    pb2_plugin->register_command(cmd_help);
    
    pb2::command cmd_list {
      .pplugin     = pb2_plugin,
      .name        = "list",
      .usage       = "| <plugin>",
      .description = "list plugins and their commands",
      .cooldown    = 0,
      .flag        = "world",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        particledi::dm_ptr dm = e->pbot->get_dm();
        std::shared_ptr<pb2::plugin_service> pl_s = dm->get<pb2::plugin_service>();
        
        if (e->split.size() == 1) {
          std::stringstream ss;
          ss << "&BPlugins:&N ";
          
          for (std::pair<std::string, pb2::plugin::ptr> p : pl_s->plugins) {
            ss << p.first << " ";
          }
          
          e->socket->stream() << pb2::ircstream::nreply(e, ss.str());
        } else if (e->split.size() == 2 && (pl_s->plugins.find(e->split[1]) != pl_s->plugins.end())) {
          std::stringstream ss;
          ss << "&B" << e->split[1] << ":&N ";
          
          for (std::pair<std::string, pb2::command> p : pl_s->plugins[e->split[1]]->commands) {
            ss << p.first << " ";
          }
          
          e->socket->stream() << pb2::ircstream::nreply(e, ss.str());
        } else {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        }
      }
    };
    pb2_plugin->register_command(cmd_list);
  }
  
  void pb2_deinit() {
    pb2_plugin->l.debug("Bye, World!");
  }

}
