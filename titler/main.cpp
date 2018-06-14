#include "../particlebot2.hpp"

extern "C" {

  pb2::plugin* pb2_plugin;

  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {}
  
  void pb2_deinit() {}
  
}
