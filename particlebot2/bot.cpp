#include "bot.hpp"

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

    bot& pub;

    std::shared_ptr<config_service> config_s;
    std::shared_ptr<db_service> db_s;
    std::shared_ptr<help_service> help_s;
  };

  /*
   * Private implementation
   */

  bot_private::bot_private(bot& _pub, particledi::dm_ptr dm)
  : pub(_pub)
  , config_s(dm->get<config_service>())
  , db_s(dm->get<db_service>())
  , help_s(dm->get<help_service>())
  {}

  /*
   * Public implementation
   */

  bot::bot(particledi::dm_ptr dm)
  : priv(new bot_private(*this, dm))
  {

  }

}