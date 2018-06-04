#include "bot.hpp"

#include "config.hpp"

#include "config_service.hpp"
#include "db_service.hpp"
#include "help_service.hpp"

namespace pb2 {

  /*
   * Private declaration
   */

  class bot_private {
  public:
    bot_private(bot& pub, particledi::dm_ptr dm);

    void load_config();

    bot& pub;

    std::shared_ptr<config_service> config_s;
    std::shared_ptr<db_service> db_s;
    std::shared_ptr<help_service> help_s;

    std::string prefix;
  };

  /*
   * Private implementation
   */

  bot_private::bot_private(bot& _pub, particledi::dm_ptr dm)
  : pub(_pub)
  , config_s(dm->get<config_service>())
  , db_s(dm->get<db_service>())
  , help_s(dm->get<help_service>())
  {
    load_config();
  }

  void bot_private::load_config() {
    config_t cfg = config_s->get_config();

    prefix = cfg.prefix;
  }

  /*
   * Public implementation
   */

  bot::bot(particledi::dm_ptr dm)
  : priv(new bot_private(*this, dm))
  {
    dm->set<bot>(this);
  }

  bot::~bot() {}

}