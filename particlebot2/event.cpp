#include "event.hpp"

namespace pb2 {
  event::event() {}
  event::~event() {}
  
  event_test::event_test(bot::ptr _bot)
  : pbot(_bot)
  {}
  
  event_test::~event_test() {}
  
  event::ptr event_test::create(bot::ptr bot) {
    return event::ptr(dynamic_cast<event*>(new event_test(bot))); 
  }
  
}