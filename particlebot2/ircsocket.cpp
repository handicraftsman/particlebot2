#include "ircsocket.hpp"

#include <queue>

namespace pb2 {

  /*
   * Private declaration
   */

  class ircsocket_private {
  public:
    ircsocket_private(ircsocket& pub, particledi::dm_ptr dm);

    void enqueue(std::string& msg);

    ircsocket& pub;
    
    std::queue<std::string> q;
  };

  /*
   * Private implementation
   */

  ircsocket_private::ircsocket_private(ircsocket& _pub, particledi::dm_ptr dm)
  : pub(_pub)
  {}

  void ircsocket_private::enqueue(std::string& msg) {
    q.push(msg);
  }

  /*
   * Public implementation
   */

  ircsocket::ircsocket(particledi::dm_ptr dm)
  : priv(new ircsocket_private(*this, dm))
  {}

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