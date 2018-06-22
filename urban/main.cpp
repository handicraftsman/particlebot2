#include "../particlebot2.hpp"

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <cstdlib>

#include <json/json.h>

#include <curl/curl.h>

class Query;

struct definition {
public:
  std::string def;
  std::string example;
  std::string author;
  std::string permalink;
  int thumbs_up;
  int thumbs_down;
  int score;
  
  bool operator<(const definition& other);
};

bool definition::operator<(const definition& other) {
  return score < other.score;
}

class query {
public:
  query(std::string term);

  std::string term;
  std::vector<definition> definitions;
  
private:
  std::string send(std::string url);
  void parse(std::string json);
};

query::query(std::string _term) : term(_term) {
  std::string t;

  for (char c : term) {
    if (c == ' ' || c == '\t' || c == '&') t += '-';
    if (c == '\r' || c == '\n') t += '\0';
    if (c >= '0') t += c;
  }
  const std::string url = "http://api.urbandictionary.com/v0/define?term=" + t;

  parse(send(url));
}

static unsigned writer(char* data, size_t size, size_t nmemb, std::string* writer) {
  if (writer == NULL)
    return 0;
  writer->append(data, size * nmemb);
  if (writer->size() >= 5000000) {
    return -1;
  } else {
    return size * nmemb;
  }
}

std::string query::send(std::string url) {
  std::string buf;
  char ebuf[CURL_ERROR_SIZE];

  CURL* curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL,             url.c_str());
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL,        true);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,  true);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS,       5);
  curl_easy_setopt(curl, CURLOPT_RANGE,           "0-5000000");
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,         30);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA,      NULL);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,   writer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,       &buf);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER,     ebuf);

  CURLcode code = curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  if (code != CURLE_OK) throw std::runtime_error("CURL error: failed to get `" + url + "` [" + ebuf + "]");

  return buf;
}

void query::parse(std::string json) {
  Json::Value root;
  std::string errs;

  Json::CharReaderBuilder b;
  std::stringstream s(json);
  bool ok = parseFromStream(b, s, &root, &errs);
  if (!ok) throw std::runtime_error("JSON error: " + errs);

  if (!root.isObject()) throw std::runtime_error("JSON error: expected Object, got something else");
  for (auto val : root["list"]) {
    if (val.isObject()) {
      definition def;

      #define rep(s) std::regex_replace(std::regex_replace(s, std::regex("[\r\n]*$"), " "), std::regex("[\r]?\n[\r\n]*"), " ")

      def.def = rep(val["definition"].asString());
      def.example = rep(val["example"].asString());
      def.author = val["author"].asString();
      def.permalink = val["permalink"].asString();
      def.thumbs_up = val["thumbs_up"].asInt();
      def.thumbs_down = val["thumbs_down"].asInt();
      def.score = def.thumbs_up - def.thumbs_down;

      #undef rep

      definitions.push_back(def);
    }
  }
}

extern "C" {
  pb2::plugin* pb2_plugin;
  
  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    pb2::command urban_cmd {
      .pplugin     = pb2_plugin,
      .name        = "urban",
      .usage       = "<definition...>",
      .description = "looks given definition up in urban dictionary",
      .cooldown    = 10,
      .flag        = "enable",
      .handler     = [] (pb2::command& c, pb2::event_command::ptr e) {
        if (e->split.size() < 2) {
          e->socket->stream() << pb2::ircstream::nreply(e, "Invalid arguments!");
          return;
        }
        
        std::stringstream ss;
        for (int i = 1; i < e->split.size(); ++i) {
          ss << e->split[i] << " ";
        }
        std::string term = ss.str();
        term.pop_back();
        
        query q(term);
        
        if (q.definitions.size() > 0) {
          definition def = *std::max_element(q.definitions.begin(), q.definitions.end());
          e->socket->stream() << pb2::ircstream::reply(e, "&B" + term + ":&N " + def.def);
        } else {
          e->socket->stream() << pb2::ircstream::reply(e, "No definitions available");
        }
      }
    };
    pb2_plugin->register_command(urban_cmd);
  }
  
  void pb2_deinit() {}
  
}
