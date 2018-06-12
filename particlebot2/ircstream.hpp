#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "ircsocket_base.hpp"

namespace pb2 {

  class ircsocket_base;
  class ircsocket_test;

  class ircstream : public std::ostream, std::streambuf {
  public:
    friend ircsocket_test;

    ircstream(ircsocket_base& socket);
    virtual ~ircstream();
  
    static std::string nick(std::string nickname);
    static std::string auth_pass(std::string password);
    static std::string auth_user(std::string username, std::string realname);


    static std::string privmsg(std::string target, std::string message);
    static std::string notice(std::string target, std::string message);

    static std::string ctcp(std::string target, std::string type, std::string message);
    static std::string nctcp(std::string target, std::string type, std::string message);

    static std::string join(std::string channel);
    static std::string join(std::string channel, std::string password);
    
    static std::string part(std::string channel, std::string reason = "Bye!");

    static std::string who(std::string channel);

    template<class E>
    static std::string reply(E e, std::string message) {
      if (e->target == e->socket->get_config().nick) {
        return privmsg(e->nick, message);
      } else {
        return privmsg(e->target, message);
      }
    }
    
    template<class E>
    static std::string nreply(E e, std::string message) {
      return notice(e->nick, message);
    }
    
  protected:
    std::streambuf::int_type overflow(std::streambuf::int_type c);

  private:
    ircsocket_base& socket;
    std::stringstream buf;
    int prev;
  };

}
