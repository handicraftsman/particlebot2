#include "bot.hpp"

#include <map>

#include "config.hpp"

#include "config_service.hpp"
#include "db_service.hpp"
#include "plugin_service.hpp"

#include "ircsocket.hpp"

#include <guosh.hpp>

namespace pb2 {

  /*
   * Private declaration
   */

  class bot_private {
  public:
    bot_private(bot& pub, particledi::dm_ptr dm);

    void load_config();

    std::optional<std::shared_ptr<ircsocket_base>>
    get_socket(std::string& name);

    bot& pub;

    std::weak_ptr<particledi::dm> dm_weak;

    Guosh::Logger l;

    std::shared_ptr<config_service> config_s;
    std::shared_ptr<db_service> db_s;

    std::string prefix;

    std::map<std::string, std::shared_ptr<ircsocket_base>> sockets;
  };

  /*
   * Private implementation
   */

  bot_private::bot_private(bot& _pub, particledi::dm_ptr _dm)
  : pub(_pub)
  , dm_weak(std::weak_ptr<particledi::dm>(_dm))
  , l("bot")
  , config_s(_dm->get<config_service>())
  , db_s(_dm->get<db_service>())
  {}

  void bot_private::load_config() {
    config_t cfg = config_s->get_config();

    prefix = cfg.prefix;

    for (std::pair<std::string, config_server_t> sp : cfg.servers) {
      sockets[sp.first] = std::shared_ptr<ircsocket_base>(
        dynamic_cast<ircsocket_base*>(
          new ircsocket(dm_weak, sp.first, sp.second)
        )
      );
    }
  }

  std::optional<std::shared_ptr<ircsocket_base>>
  bot_private::get_socket(std::string& name) {
    if (sockets.find(name) != sockets.end()) {
      return sockets[name];
    } else {
      return std::nullopt;
    }
  }

  /*
   * Public implementation
   */

  bot::bot(particledi::dm_ptr dm)
  : priv(new bot_private(*this, dm))
  {
    dm->set<bot>(this);
    priv->l.debug("Initialized pb2::bot and pb2::bot_private");
  }

  bot::~bot() {}

  void bot::load_config() {
    priv->load_config();
  }

  std::optional<std::shared_ptr<ircsocket_base>>
  bot::get_socket(std::string& name) {
    return priv->get_socket(name);
  }

}