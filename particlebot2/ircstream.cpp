#include "ircstream.hpp"

#include <vector>
#include <locale>

pb2::ircstream::ircstream(pb2::ircsocket_base& _socket)
: std::ostream(this)
, socket(_socket)
{}

pb2::ircstream::~ircstream() {}

std::streambuf::int_type pb2::ircstream::overflow(std::streambuf::int_type c) {
  if (c != EOF) {
    buf.put(static_cast<char>(c));
    if (prev == '\r' && c == '\n') {
      std::string msg = buf.str();
      socket.enqueue(msg);
      buf.str("");
    }
  }
  prev = c;
  return 0;
}

std::string pb2::ircstream::nick(std::string nickname) {
  return "NICK " + nickname + "\r\n";
}

std::string pb2::ircstream::auth_pass(std::string password) {
  return "PASS " + password + "\r\n";
}

std::string pb2::ircstream::auth_user(std::string username, std::string realname) {
  return "USER " + username + " 0 * :" + realname + "\r\n";
}

static void replace_all(std::string& str, const std::string& from, const std::string& to) {
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

static std::vector<std::string> split_every(std::string& s, size_t e) {
  std::vector<std::string> o;
  std::string b;

  size_t i = 0;
  for (char c : s) {
    if (i == e || c == '\n') {
      o.push_back(b);
      b = "";
      i = 0;
    }
    if (c != '\n') b += c;
    ++i;
  }
  
  if (!b.empty())
    o.push_back(b);

  return o;
}

static void format(std::string& message) {
  static std::vector<std::pair<std::string, std::string>> rules {
    std::pair<std::string, std::string>("&N",      "\x0F"),
    std::pair<std::string, std::string>("&B",      "\x02"),
    std::pair<std::string, std::string>("&U",      "\x1F"),
    std::pair<std::string, std::string>("&I",      "\x10"),
    std::pair<std::string, std::string>("%C",      "\x03"),
    std::pair<std::string, std::string>("?WHITE",  "0"),
    std::pair<std::string, std::string>("?BLACK",  "1"),
    std::pair<std::string, std::string>("?BLUE",   "2"),
    std::pair<std::string, std::string>("?GREEN",  "3"),
    std::pair<std::string, std::string>("?RED",    "4"),
    std::pair<std::string, std::string>("?BROWN",  "5"),
    std::pair<std::string, std::string>("?PURPLE", "6"),
    std::pair<std::string, std::string>("?ORANGE", "7"),
    std::pair<std::string, std::string>("?YELLOW", "8"),
    std::pair<std::string, std::string>("?LGREEN", "9"),
    std::pair<std::string, std::string>("?CYAN",   "10"),
    std::pair<std::string, std::string>("?LCYAN",  "11"),
    std::pair<std::string, std::string>("?LBLUE",  "12"),
    std::pair<std::string, std::string>("?PINK",   "13"),
    std::pair<std::string, std::string>("?GREY",   "14"),
    std::pair<std::string, std::string>("?LGREY",  "15")
  };
  for (auto p : rules) {
    replace_all(message, p.first, p.second);
  }
}

std::string pb2::ircstream::privmsg(std::string target, std::string message) {
  std::stringstream ss;
  
  format(message);
  for (std::string str : split_every(message, 400)) {
    ss << ("PRIVMSG " + target + " :" + str + "\r\n");
  }

  return ss.str();
}

std::string pb2::ircstream::notice(std::string target, std::string message) {
  std::stringstream ss;

  format(message);
  for (std::string str : split_every(message, 400)) {
    ss << ("NOTICE " + target + " :" + str + "\r\n");
  }

  return ss.str();
}

std::string pb2::ircstream::ctcp(std::string target, std::string type, std::string message) {
  return pb2::ircstream::privmsg(target, "\x01" + type + " " + message + "\x01");
}

std::string pb2::ircstream::nctcp(std::string target, std::string type, std::string message) {
  return pb2::ircstream::notice(target, "\x01" + type + " " + message + "\x01");
}

std::string pb2::ircstream::join(std::string channel) {
  return ("JOIN " + channel + "\r\n");
}

std::string pb2::ircstream::join(std::string channel, std::string password) {
  return ("JOIN " + channel + " " + password + "\r\n");
}

std::string pb2::ircstream::part(std::string channel, std::string reason) {
  return ("PART " + channel + " :" + reason + "\r\n");
}

std::string pb2::ircstream::who(std::string channel) {
  return ("WHO " + channel + "\r\n");
}
