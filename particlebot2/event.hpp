#pragma once

#include <memory>
#include <vector>

#include "bot.hpp"

namespace pb2 {
  
  class bot;
  typedef bot* _event_bot_ptr;

  class event {
  public:
    typedef std::shared_ptr<event> ptr;
    
    event();
    virtual ~event();
  };
  
  class event_test : event {
  private:
    event_test(_event_bot_ptr bot);
  
  public:
    typedef std::shared_ptr<event_test> ptr;
    
    static event::ptr create(_event_bot_ptr bot);
    virtual ~event_test();
    
    _event_bot_ptr pbot;
  };
  
  class event_connect : event {
  private:
    event_connect(_event_bot_ptr bot, ircsocket_base::cptr socket);
    
  public:
    typedef std::shared_ptr<event_connect> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket);
    virtual ~event_connect();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
  };
  
  class event_disconnect : event {
  private:
    event_disconnect(_event_bot_ptr bot, ircsocket_base::cptr socket);
    
  public:
    typedef std::shared_ptr<event_disconnect> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket);
    virtual ~event_disconnect();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
  };
  
  class event_message : event {
  private:
    event_message(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& message);
    
  public:
    typedef std::shared_ptr<event_message> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& message);
    virtual ~event_message();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string message;
  };
  
  class event_code : event {
  private:
    event_code(_event_bot_ptr bot, ircsocket_base::cptr socket, int code, std::string& extra);
    
  public:
    typedef std::shared_ptr<event_code> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, int code, std::string& extra);
    virtual ~event_code();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    int code;
    std::string extra;
  };
  
  class event_ping : event {
  private:
    event_ping(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& target);
    
  public:
    typedef std::shared_ptr<event_ping> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& target);
    virtual ~event_ping();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string target;
  };
  
  class event_whoreply : event {
  private:
    event_whoreply(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host);
    
  public:
    typedef std::shared_ptr<event_whoreply> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host);
    virtual ~event_whoreply();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
  };
  
  class event_join : event {
  private:
    event_join(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel);
    
  public:
    typedef std::shared_ptr<event_join> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel);
    virtual ~event_join();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string channel;
  };
  
  class event_part : event {
  private:
    event_part(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel, std::string& reason);
    
  public:
    typedef std::shared_ptr<event_part> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& channel, std::string& reason);
    virtual ~event_part();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string channel;
    std::string reason;
  };
  
  class event_nick : event {
  private:
    event_nick(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& new_nick);
    
  public:
    typedef std::shared_ptr<event_nick> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& new_nick);
    virtual ~event_nick();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string new_nick;
  };
  
  class event_privmsg : event {
  private:
    event_privmsg(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& message);
    
  public:
    typedef std::shared_ptr<event_privmsg> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& message);
    virtual ~event_privmsg();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string target;
    std::string message;
  };
  
  class event_ctcp : event {
  private:
    event_ctcp(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& type, std::string& message);
    
  public:
    typedef std::shared_ptr<event_ctcp> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& target, std::string& host, std::string& type, std::string& message);
    virtual ~event_ctcp();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string target;
    std::string type;
    std::string message;
  };
  
  class event_notice : event {
  private:
    event_notice(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& message);
    
  public:
    typedef std::shared_ptr<event_notice> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& message);
    virtual ~event_notice();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string target;
    std::string message;
  };
  
  class event_nctcp : event {
  private:
    event_nctcp(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& type, std::string& message);
    
  public:
    typedef std::shared_ptr<event_nctcp> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& target, std::string& host, std::string& type, std::string& message);
    virtual ~event_nctcp();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string target;
    std::string type;
    std::string message;
  };
  
  class event_command : event {
  private:
    event_command(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& cmd, std::vector<std::string>& split);
    
  public:
    typedef std::shared_ptr<event_command> ptr;
    
    static event::ptr create(_event_bot_ptr bot, ircsocket_base::cptr socket, std::string& nick, std::string& user, std::string& host, std::string& target, std::string& cmd, std::vector<std::string>& split);
    virtual ~event_command();
    
    _event_bot_ptr pbot;
    ircsocket_base::cptr socket;
    std::string nick;
    std::string user;
    std::string host;
    std::string target;
    std::string cmd;
    std::vector<std::string> split;
  };
}