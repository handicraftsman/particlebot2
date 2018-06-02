/*
#include "config.hpp"

#include <regex>

#include <particleini.hpp>

using namespace std::regex_constants;

static void add_server(
  pb2::config& cfg,
  std::string name,
  particleini::config::dsec& sect
) {
  for (std::pair<std::string, std::string> line : sect) {
    // todo
  }
}

std::regex regex_server("server/(.+)", ECMAScript | icase);

pb2::config::config(std::string path) {
  //static std::regex 
  
  particleini::config cfg(path);

  for (std::pair<std::string, particleini::config::dsec> sp : cfg.data) {
    std::string& name = sp.first;
    particleini::config::dsec& sect = sp.second;
  
    
    auto m = std::smatch {};
  
    if ((m = std::smatch {}, std::regex_match(name, m, regex_server))) {
	    add_server(cfg, m[1], sect);
    }
  }
  
}

pb2::config::~config() {}
*/