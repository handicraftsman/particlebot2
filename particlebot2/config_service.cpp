#include "config_service.hpp"

#include <iostream>
#include <regex>

#include <particleini.hpp>

using namespace std::regex_constants;

std::regex regex_addr("^(.+?)/([+])?(\\d{1,5})$", ECMAScript | icase);

static void add_server_address(
  pb2::config_server_t& server,
  std::string& astr
) {
  pb2::config_address_t addr;
  
  auto m = std::smatch {};

  if (std::regex_match(astr, m, regex_addr)) {
    addr.host = m[1];
    addr.port = std::stoi(m[3]); 
  }

  server.addresses.push_back(addr);
}

static void add_server(
  pb2::config_t& cfg,
  std::string name,
  particleini::config::dsec& sect
) {
  pb2::config_server_t server;

  for (std::pair<std::string, std::string> line : sect) {
    std::string& name = line.first;
    std::string& value = line.second;
    if (name == "addr") {
      add_server_address(server, value);
    } else if (name == "nick") {
      server.nick = value;
    } else if (name == "user") {
      server.user = value;
    } else if (name == "pass") {
      server.pass = value;
    } else if (name == "rnam") {
      server.rnam = value;
    } else if (name == "join") {
      server.autojoin.push_back(value);
    } else {
      std::cerr << "Invalid property: " << name << std::endl;
    }
  }

  cfg.servers[name] = server;
}

static void add_plugin(
  pb2::config_t& cfg,
  std::string name,
  particleini::config::dsec& sect
) {
  pb2::config_plugin_t plugin;

  plugin.config = sect;

  cfg.plugins[name] = plugin;
}

static void add_group(
  pb2::config_t& cfg,
  std::string name,
  particleini::config::dsec& sect
) {
  pb2::config_group_t group;

  group.flags = sect;

  cfg.groups[name] = group;
}

std::regex regex_server("^server/(.+)$", ECMAScript | icase);
std::regex regex_plugin("^plugin/(.+)$", ECMAScript | icase);
std::regex regex_group("^group/(.+)$", ECMAScript | icase);

pb2::config_service::config_service(std::string _path)
: path(_path)
{}

pb2::config_service::~config_service() {}

pb2::config_t pb2::config_service::get_config() {
  pb2::config_t cfg;

  particleini::config ini(path);

  for (std::pair<std::string, particleini::config::dsec> sp : ini.data) {
    std::string& name = sp.first;
    particleini::config::dsec& sect = sp.second;

    auto m = std::smatch {};
  
    if (name == "general") {
      for (std::pair<std::string, std::string> line : sect) {
        if (line.first == "prefix") {
          cfg.prefix = line.second;
        }
      }
    } else if ((m = std::smatch {}, std::regex_match(name, m, regex_server))) {
	    add_server(cfg, m[1], sect);
    } else if ((m = std::smatch {}, std::regex_match(name, m, regex_plugin))) {
      add_plugin(cfg, m[1], sect);
    } else if ((m = std::smatch {}, std::regex_match(name, m, regex_group))) {
      add_group(cfg, m[1], sect);
    } else {
      std::cerr << "Invalid section: " << name << std::endl;
    }
  }

  if (cfg.plugins.find("core") == cfg.plugins.end()) {
    particleini::config::dsec s {};
    add_plugin(cfg, "core", s);
  }
 
  return cfg;
}