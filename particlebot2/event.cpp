#include "event.hpp"

#define make_event(e) event::ptr(dynamic_cast<event*>((e)))

namespace pb2 {
  // event
  event::event() {}
  event::~event() {}
  
  // event_test
  event_test::event_test(_event_bot_ptr _bot)
  : pbot(_bot)
  {}
  
  event_test::~event_test() {}
  
  event::ptr event_test::create(_event_bot_ptr bot) {
    return make_event(new event_test(bot)); 
  }
  
  // event_connect
  event_connect::event_connect(_event_bot_ptr _bot, ircsocket_base::cptr _socket)
  : pbot(_bot)
  , socket(_socket)
  {}
  
  event_connect::~event_connect() {}
  
  event::ptr event_connect::create(_event_bot_ptr bot, ircsocket_base::cptr socket) {
    return make_event(new event_connect(bot, socket));
  }
  
  // event_disconnect
  event_disconnect::event_disconnect(_event_bot_ptr _bot, ircsocket_base::cptr _socket)
  : pbot(_bot)
  , socket(_socket)
  {}
  
  event_disconnect::~event_disconnect() {}
  
  event::ptr event_disconnect::create(_event_bot_ptr bot, ircsocket_base::cptr socket) {
    return make_event(new event_disconnect(bot, socket));
  }
  
  // event_message
  event_message::event_message(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _message)
  : pbot(_bot)
  , socket(_socket)
  , message(_message)
  {}
  
  event_message::~event_message() {}
  
  event::ptr event_message::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& message) {
    return make_event(new event_message(bot, socket, message)); 
  }
  
  // event_code
  event_code::event_code(_event_bot_ptr _bot, ircsocket_base::cptr _socket, int _code, std::string& _extra)
  : pbot(_bot)
  , socket(_socket)
  , code(_code)
  , extra(_extra)
  {}
  
  event_code::~event_code() {}
  
  event::ptr event_code::create(_event_bot_ptr bot, ircsocket_base::cptr socket, int code, std::string& extra) {
    return make_event(new event_code(bot, socket, code, extra));
  }
  
  // event_ping
  event_ping::event_ping(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _target)
  : pbot(_bot)
  , socket(_socket)
  , target(_target)
  {}
  
  event_ping::~event_ping() {}
  
  event::ptr event_ping::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& target) {
    return make_event(new event_ping(bot, socket, target));
  }
  
  // event_whoreply
  event_whoreply::event_whoreply(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  {}
 
  event_whoreply::~event_whoreply() {}
  
  event::ptr event_whoreply::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host) {
    return make_event(new event_whoreply(bot, socket, nick, user, host));
  }
 
  // event_join
  event_join::event_join(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _channel)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , channel(_channel)
  {}
  
  event_join::~event_join() {}
  
  event::ptr event_join::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel) {
    return make_event(new event_join(bot, socket, nick, user, host, channel));
  }
  
  // event_part
  event_part::event_part(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _channel, std::string& _reason)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , channel(_channel)
  , reason(_reason)
  {}
  
  event_part::~event_part() {}
  
  event::ptr event_part::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel, std::string& reason) {
    return make_event(new event_part(bot, socket, nick, user, host, channel, reason));
  }
  
  // event_nick
  event_nick::event_nick(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _new_nick)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , new_nick(_new_nick)
  {}
  
  event_nick::~event_nick() {}
  
  event::ptr event_nick::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &new_nick) {
    return make_event(new event_nick(bot, socket, nick, user, host, new_nick));
  }
  
  // event_privmsg
  event_privmsg::event_privmsg(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _target, std::string& _message)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , target(_target)
  , message(_message)
  {}
  
  event_privmsg::~event_privmsg() {}
  
  event::ptr event_privmsg::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &target, std::string &message) {
    return make_event(new event_privmsg(bot, socket, nick, user, host, target, message));
  }
  
  // event_ctcp
  event_ctcp::event_ctcp(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _target, std::string& _type, std::string& _message)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , target(_target)
  , type(_type)
  , message(_message)
  {}
  
  event_ctcp::~event_ctcp() {}
  
  event::ptr event_ctcp::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &target, std::string& type, std::string &message) {
    return make_event(new event_ctcp(bot, socket, nick, user, host, target, type, message));
  }

  // event_notice
  event_notice::event_notice(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _target, std::string& _message)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , target(_target)
  , message(_message)
  {}
  
  event_notice::~event_notice() {}
  
  event::ptr event_notice::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &target, std::string &message) {
    return make_event(new event_notice(bot, socket, nick, user, host, target, message));
  }
  
  // event_nctcp
  event_nctcp::event_nctcp(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _target, std::string& _type, std::string& _message)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , target(_target)
  , type(_type)
  , message(_message)
  {}
  
  event_nctcp::~event_nctcp() {}
  
  event::ptr event_nctcp::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &target, std::string& type, std::string &message) {
    return make_event(new event_nctcp(bot, socket, nick, user, host, target, type, message));
  }
  
  // event_command
  event_command::event_command(_event_bot_ptr _bot, ircsocket_base::cptr _socket, std::string& _nick, std::string& _user, std::string& _host, std::string& _target, std::string& _cmd, std::vector<std::string>& _split)
  : pbot(_bot)
  , socket(_socket)
  , nick(_nick)
  , user(_user)
  , host(_host)
  , target(_target)
  , cmd(_cmd)
  , split(_split)
  {}
  
  event_command::~event_command() {}
  
  event::ptr event_command::create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string &nick, std::string &user, std::string &host, std::string &target, std::string &cmd, std::vector<std::string> &split) {
    return make_event(new event_command(bot, socket, nick, user, host, target, cmd, split));
  }
}