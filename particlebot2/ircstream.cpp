#include "ircstream.hpp"

pb2::ircstream::ircstream(pb2::ircsocket& _socket)
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
  return 0;
}