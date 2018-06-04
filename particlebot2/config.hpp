#pragma once

#include <map>
#include <optional>
#include <vector>

namespace pb2 {

  struct config_address_t {
    std::string host;
    int port;
    bool ssl;
  };

  struct config_server_t {
    std::vector<config_address_t> addresses;
    std::string nick;
    std::string user;
    std::optional<std::string> pass;
    std::string rnam;
    std::vector<std::string> autojoin;
  };

  struct config_plugin_t {
    // A vector of key/value pairs
    std::vector<std::pair<std::string, std::string>> config;
  };

  struct config_group_t {
    // A vector of plugin / flag-name pairs
    std::vector<std::pair<std::string, std::string>> flags;
  };

  struct config_t {
    std::string prefix;
    std::map<std::string, config_server_t> servers;
    std::map<std::string, config_plugin_t> plugins;
    std::map<std::string, config_group_t> groups;
  };

}