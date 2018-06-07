#pragma once

#include <particletest.hpp>

#include <particlebot2.hpp>

namespace pb2t {

class ircsocket_test : public particletest {
public:
  ircsocket_test()
  : particletest("pb2t::ircsocket_test")
  {
    ptest_register_test("authentication works", &ircsocket_test::test_authentication_works);
  }

  virtual ~ircsocket_test() {}

  bool test_authentication_works() {
    pb2::ircsocket_fake socket;
    
    socket.stream()
      << pb2::ircstream::auth_pass("a_password")
      << pb2::ircstream::nick("ParticleBot2")
      << pb2::ircstream::auth_user("ParticleBot2", "An IRC bot in C++")
    ;

    ptest_expect(socket.get() == "PASS a_password\r\nNICK ParticleBot2\r\nUSER ParticleBot2 0 * :An IRC bot in C++\r\n");

    return true;
  }
};

}