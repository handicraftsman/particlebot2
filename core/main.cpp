#include "../particlebot2.hpp"

extern "C" {

  pb2::plugin* pb2_plugin;

  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    pb2_plugin->l.debug("Hello, World!");
    
    pb2_plugin->register_event_handler<pb2::event_test>([] (pb2::event::ptr _e) { 
      pb2::event_test::ptr e = pb2_ptrcast<pb2::event_test>(_e);
      pb2_plugin->l.debug("Received a test event");
    });
    
    pb2_plugin->register_event_handler<pb2::event_disconnect>([] (pb2::event::ptr _e) {
      pb2::event_disconnect::ptr e = pb2_ptrcast<pb2::event_disconnect>(_e);
      // reconnect
    });
  }
  
  void pb2_deinit() {
    pb2_plugin->l.debug("Bye, World!");
  }

}