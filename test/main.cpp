#include "ircsocket.hpp"

int main(int argc, char** argv) {
  return !particletest::run_all({
    pb2t::ircsocket_test()
  });
}