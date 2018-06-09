#include "ircsocket.hpp"

#include <algorithm>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <cstring>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <sys/select.h>

#include <guosh.hpp>

#include <cxxabi.h>

namespace pb2 {

  /*
   * Base implementation
   */ 

  ircsocket_base::~ircsocket_base() {}

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

    config_address_t get_next_addr();
    void connect();

    void read_loop();
    void write_loop();

    void enqueue(std::string& msg);

    ircsocket& pub;
    
    int addr_pos;
    int conn;
    
    config_server_t cfg;
    
    Guosh::Logger l;

    std::mutex read_mtx;
    std::mutex write_mtx;
   
    bool running;
    std::mutex running_mtx;
    
    std::queue<std::string> q;
    uint16_t burst;
    std::chrono::time_point<std::chrono::system_clock> last_write;
  };

  /*
   * Private implementation
   */

  ircsocket_private::ircsocket_private(
    ircsocket& _pub,
    std::weak_ptr<particledi::dm> dm,
    std::string name,
    config_server_t& _cfg
  )
  : pub(_pub)
  , addr_pos(0)
  , conn(0)
  , cfg(_cfg)
  , l("!" + name)
  , running(false)
  {
    std::thread(std::bind(&ircsocket_private::read_loop, this)).detach();
    std::thread(std::bind(&ircsocket_private::write_loop, this)).detach();
  }
  
  config_address_t ircsocket_private::get_next_addr() {
    if (addr_pos >= cfg.addresses.size()) {
      addr_pos = 0;
    }
    config_address_t addr = cfg.addresses[addr_pos];
    ++addr_pos;
    return addr;
  }
  
  void ircsocket_private::connect() {
    std::lock_guard<std::mutex> read_lock(read_mtx);
    std::lock_guard<std::mutex> write_lock(write_mtx);
    std::lock_guard<std::mutex> running_lock(running_mtx);
    
    try {
      config_address_t addr = get_next_addr();
      
      l.important("Connecting to %s:%d...", addr.host.c_str(), addr.port);
      
      struct addrinfo hints, *res;
      memset(&hints, 0, sizeof(hints));
      hints.ai_family   = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      
      if (getaddrinfo(addr.host.c_str(), std::to_string(addr.port).c_str(), &hints, &res) < 0) {
        throw std::runtime_error("Cannot resolve " + addr.host + ":" + std::to_string(addr.port));
      }
      
      conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (conn == -1) {
        throw std::runtime_error("Cannot create a socket");
      }
      
      if (::connect(conn, res->ai_addr, res->ai_addrlen) == -1) {
        throw std::runtime_error("Cannot connect to " + addr.host + ":" + std::to_string(addr.port));
      }
      
      fcntl(conn, F_SETFL, O_NONBLOCK);
      
      {
        auto stream = pub.stream();
        if (cfg.pass) {
          stream << ircstream::auth_pass(cfg.pass.value());
        }
        stream << ircstream::nick(cfg.nick);
        stream << ircstream::auth_user(cfg.nick, cfg.rnam);
      }
      
      running = true;
    } catch (std::exception& exc) {
      char* exc_name = abi::__cxa_demangle(typeid(exc).name(), nullptr, nullptr, nullptr);
      l.error("Exception: %s (%s)", exc_name, exc.what());
      free(exc_name);
    }
  }
  
  static int read_line(int fd, char* buf) {
    memset(buf, 0, 513);
    int i = 0;
    while (i < 512) {
      int ok = read(fd, &buf[i], 1);
      if (ok < 1) {
        if (errno == EAGAIN) {
          return -10;
        } else {
          return -1;
        }
      }
      if (i > 0 && buf[i-1] == '\r' && buf[i] == '\n') {
        return 0;
      }
      ++i;
    }
    return -2;
  }
  
  void ircsocket_private::read_loop() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      std::lock_guard<std::mutex> write_lock(write_mtx);
      std::lock_guard<std::mutex> running_lock(running_mtx);
      
      if (running) {
        try {
          char buf[513];
          int ok = read_line(conn, buf);
          if (ok < 0 && ok != -10) {
            throw std::runtime_error("unable to read");
          }
          if (ok != -10) {
            std::string msg(buf);
            
            std::string::size_type pos = 0;
            while ((pos = msg.find("\r\n", pos)) != std::string::npos) {
              msg.erase(pos, 2);
            }
            
            l.io("R> %s", msg.c_str());
          }
        } catch (std::exception& exc) {
          char* exc_name = abi::__cxa_demangle(typeid(exc).name(), nullptr, nullptr, nullptr);
          l.error("Exception: %s (%s)", exc_name, exc.what());
          free(exc_name);
          running = false;
        }
      }
    }
  }

  void ircsocket_private::write_loop() {
    std::string popped;
    bool wrote = true;
    
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      std::lock_guard<std::mutex> read_lock(read_mtx);
      std::lock_guard<std::mutex> running_lock(running_mtx);
      
      if (running) {
        try {
           if (burst >= 5) {
             std::this_thread::sleep_for(std::chrono::milliseconds(700));
           } else {
             ++burst;
           }
           auto current_time = std::chrono::system_clock::now();
           if (current_time - last_write >= std::chrono::milliseconds(700)) {
             burst = 0;
           }
           if (!q.empty()) {
             
             std::string to_write; 
             if (wrote == true) {
               popped = q.front();
               q.pop();  
             }
             
             int ok = ::write(conn, popped.c_str(), popped.size());
             if (ok < 1) {
               if (errno == EAGAIN) {
                 wrote = false;
               } else {
                 throw std::runtime_error("unable to write");
               }
             } else {
               std::string::size_type pos = 0;
               while ((pos = popped.find("\r\n", pos)) != std::string::npos) {
                 popped.erase(pos, 2);
               }
               l.io("W> " + popped);
             }
             
             last_write = current_time;
          }
        } catch (std::exception& exc) {
          char* exc_name = abi::__cxa_demangle(typeid(exc).name(), nullptr, nullptr, nullptr);
          l.error("Exception: %s (%s)", exc_name, exc.what());
          free(exc_name);
          running = false;
          wrote = true;
        }
      }
    }
  }

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

  void ircsocket::connect() {
    priv->connect();
  }

  void ircsocket::enqueue(std::string& msg) {
    priv->enqueue(msg);
  }

}