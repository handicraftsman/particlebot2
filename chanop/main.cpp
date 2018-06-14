#include "../particlebot2.hpp"

#include <functional>
#include <string>
#include <sstream>

using namespace std::placeholders;

extern "C" {

  pb2::plugin* pb2_plugin;

  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    
    enum class GetRidType {
      Kick,
      Remove
    };
    
    auto get_rid_handler = [] (GetRidType grt, bool should_ban, pb2::command& c, pb2::event_command::ptr e) {
      if (e->split.size() < 2) {
        e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
        return;
      }
      std::string who = e->split[1];
      std::string reason = "Bye!";
      if (e->split.size() > 2) {
        std::stringstream ss;
        for (int i = 2; i < e->split.size(); ++i) {
          ss << e->split[i] << " ";
        }
        reason = ss.str();
      }
      if (should_ban) {
        std::string host = e->socket->get_user_cache()[e->split[1]].host.value_or(e->split[1]);
        e->socket->stream() << ("MODE " + e->target + " +b " + host + "\r\n");
      }
      if (grt == GetRidType::Kick) {
        e->socket->stream() << ("KICK " + e->target + " " + who + " :" + reason + "\r\n");
      } else if (grt == GetRidType::Remove) {
        e->socket->stream() << ("REMOVE " + e->target + " " + who + " :" + reason + "\r\n");
      }
       e->socket->stream() << pb2::ircstream::nreply(e, "Done!");
    };
    
    pb2::command cmd_kick {
      .pplugin     = pb2_plugin,
      .name        = "kick",
      .usage       = "<who> <reason...>",
      .description = "kicks users",
      .cooldown    = 0,
      .flag        = "kick",
      .handler     = std::bind(get_rid_handler, GetRidType::Kick, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_kick);
    
    pb2::command cmd_kban {
      .pplugin     = pb2_plugin,
      .name        = "kban",
      .usage       = "<who> <reason...>",
      .description = "kicks and bans users",
      .cooldown    = 0,
      .flag        = "kban",
      .handler     = std::bind(get_rid_handler, GetRidType::Kick, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_kban);
    
    pb2::command cmd_remove {
      .pplugin     = pb2_plugin,
      .name        = "remove",
      .usage       = "<who> <reason...>",
      .description = "removes users",
      .cooldown    = 0,
      .flag        = "remove",
      .handler     = std::bind(get_rid_handler, GetRidType::Remove, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_remove);
    
    pb2::command cmd_rban {
      .pplugin     = pb2_plugin,
      .name        = "rban",
      .usage       = "<who> <reason...>",
      .description = "removes and bans users",
      .cooldown    = 0,
      .flag        = "rban",
      .handler     = std::bind(get_rid_handler, GetRidType::Remove, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_rban);
    
    auto mode_handler = [] (
      std::string mode,
      bool give,
      bool resolve_host,
      bool allow_self,
      pb2::command& c,
      pb2::event_command::ptr e
    ) {
      std::string who = e->nick;
      if (allow_self) {
        if (e->split.size() > 2) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        if (e->split.size() == 1) {
          if (resolve_host) { who = e->host; }
        } else if (e->split.size() == 2) {
          if (resolve_host) {
            who = resolve_host ? e->socket->get_user_cache()[e->split[1]].host.value_or(e->split[1]) : e->split[1];
          } else {
            who = e->split[1];
          }
        }
      } else {
        if (e->split.size() != 2) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        if (resolve_host) {
          who = resolve_host ? e->socket->get_user_cache()[e->split[1]].host.value_or(e->split[1]) : e->split[1];
        } else {
          who = e->split[1];
        }
      }
      std::string cmd = "MODE " + e->target + " " + (give ? "+" : "-") + mode + " " + who + "\r\n";
      e->socket->stream() << cmd << pb2::ircstream::nreply(e, "Done!");
    };
    
    pb2::command cmd_ban {
      .pplugin     = pb2_plugin,
      .name        = "ban",
      .usage       = "<who>",
      .description = "bans users",
      .cooldown    = 0,
      .flag        = "ban",
      .handler     = std::bind(mode_handler, "b", true, true, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_ban);
    
    pb2::command cmd_unban {
      .pplugin     = pb2_plugin,
      .name        = "unban",
      .usage       = "<who>",
      .description = "unbans users",
      .cooldown    = 0,
      .flag        = "ban",
      .handler     = std::bind(mode_handler, "b", false, true, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_unban);
    
    pb2::command cmd_quiet {
      .pplugin     = pb2_plugin,
      .name        = "quiet",
      .usage       = "<who>",
      .description = "quiets users",
      .cooldown    = 0,
      .flag        = "quiet",
      .handler     = std::bind(mode_handler, "q", true, true, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_quiet);
    
    pb2::command cmd_unquiet {
      .pplugin     = pb2_plugin,
      .name        = "unquiet",
      .usage       = "<who>",
      .description = "unquiets users",
      .cooldown    = 0,
      .flag        = "quiet",
      .handler     = std::bind(mode_handler, "q", false, true, false, _1, _2)
    };
    pb2_plugin->register_command(cmd_unquiet);
    
    pb2::command cmd_exempt {
      .pplugin     = pb2_plugin,
      .name        = "exempt",
      .usage       = "<who>",
      .description = "exempts users",
      .cooldown    = 0,
      .flag        = "exempt",
      .handler     = std::bind(mode_handler, "e", true, true, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_exempt);
    
    pb2::command cmd_unexempt {
      .pplugin     = pb2_plugin,
      .name        = "unexempt",
      .usage       = "<who>",
      .description = "unexempts users",
      .cooldown    = 0,
      .flag        = "exempt",
      .handler     = std::bind(mode_handler, "e", false, true, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_unexempt);
  
    pb2::command cmd_op {
      .pplugin     = pb2_plugin,
      .name        = "op",
      .usage       = "<who>",
      .description = "ops users",
      .cooldown    = 0,
      .flag        = "op",
      .handler     = std::bind(mode_handler, "o", true, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_op);
    
    pb2::command cmd_deop {
      .pplugin     = pb2_plugin,
      .name        = "deop",
      .usage       = "<who>",
      .description = "deops users",
      .cooldown    = 0,
      .flag        = "op",
      .handler     = std::bind(mode_handler, "o", false, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_deop);
  
    pb2::command cmd_hop {
      .pplugin     = pb2_plugin,
      .name        = "hop",
      .usage       = "<who>",
      .description = "hops users",
      .cooldown    = 0,
      .flag        = "hop",
      .handler     = std::bind(mode_handler, "h", true, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_hop);
    
    pb2::command cmd_dehop {
      .pplugin     = pb2_plugin,
      .name        = "dehop",
      .usage       = "<who>",
      .description = "dehops users",
      .cooldown    = 0,
      .flag        = "hop",
      .handler     = std::bind(mode_handler, "h", false, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_dehop);
    
    pb2::command cmd_voice {
      .pplugin     = pb2_plugin,
      .name        = "voice",
      .usage       = "<who>",
      .description = "voices users",
      .cooldown    = 0,
      .flag        = "voice",
      .handler     = std::bind(mode_handler, "v", true, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_voice);
    
    pb2::command cmd_devoice {
      .pplugin     = pb2_plugin,
      .name        = "devoice",
      .usage       = "<who>",
      .description = "devoices users",
      .cooldown    = 0,
      .flag        = "voice",
      .handler     = std::bind(mode_handler, "v", false, false, true, _1, _2)
    };
    pb2_plugin->register_command(cmd_devoice);
  }
    
  void pb2_deinit() {}
  
}
