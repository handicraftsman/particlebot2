#include "ircsocket.hpp"
#include "ircsocket_fake.hpp"

#include <queue>

#include <guosh.hpp>

namespace pb2 {

  /*
   * Base implementation
   */ 

  ircsocket_base::~ircsocket_base() {}

  /*
   * Fake implementation
   */

  ircsocket_fake::ircsocket_fake() {}

  ircstream ircsocket_fake::stream() {
    return ircstream(*this);
  }

  std::string ircsocket_fake::get() {
    return ss.str();
  }

  void ircsocket_fake::clear() {
    ss.str("");
  }

  void ircsocket_fake::enqueue(std::string& msg) {
    ss << msg;
  }

  /*
   * Private declaration
   */

  class ircsocket_private {
  public:
    ircsocket_private(
      ircsocket& pub,
      std::weak_ptr<particledi::dm> dm,
      std::string name,
      config_server_t& cfg
    );

    void enqueue(std::string& msg);

    ircsocket& pub;
    
    Guosh::Logger l;

    std::queue<std::string> q;
  };

  /*
   * Private implementation
   */

  ircsocket_private::ircsocket_private(
    ircsocket& _pub,
    std::weak_ptr<particledi::dm> dm,
    std::string name,
    config_server_t& cfg
  )
  : pub(_pub)
  , l("!" + name)
  {}

  void ircsocket_private::enqueue(std::string& msg) {
    q.push(msg);
  }

  /*
   * Public implementation
   */

  ircsocket::ircsocket(std::weak_ptr<particledi::dm> dm, std::string name, config_server_t& cfg)
  : priv(new ircsocket_private(*this, dm, name, cfg))
  {
    priv->l.debug("Initialized pb2::ircsocket and pb2::ircsocket_private");
  }

  ircsocket::ircsocket() {}

  ircsocket::~ircsocket() {}

  ircstream ircsocket::stream() {
    return ircstream(*this);
  }

  /*
   * Protected implementation
   */
  
  void ircsocket::enqueue(std::string& msg) {
    priv->enqueue(msg);
  }

}