#include "../particlebot2.hpp"

#include <regex>

#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml++/libxml++.h>

#include <curl/curl.h>

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

bool send(std::string url, std::string& buf) {
  char ebuf[CURL_ERROR_SIZE];

  CURL* curl = curl_easy_init();

  struct curl_slist* headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: text/html; charset=utf-8");
  curl_easy_setopt(curl, CURLOPT_URL,             url.c_str());
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL,        true);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,  true);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS,       5);
  curl_easy_setopt(curl, CURLOPT_RANGE,           "0-5000000");
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,         30);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA,      NULL);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER,      headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,   writer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,       &buf);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER,     ebuf);
  
  CURLcode code = curl_easy_perform(curl);
  
  char* response;
  int result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &response);
  std::string r(response);
  
  curl_easy_cleanup(curl);

  if (code != CURLE_OK) throw std::runtime_error("CURL error: failed to get `" + url + "` [" + ebuf + "]");
  
  if (r.substr(0, 9) == "text/html") {
    return true;
  }
  
  return false;
}

extern "C" {
  pb2::plugin* pb2_plugin;

  void pb2_init(std::vector<std::pair<std::string, std::string>>& cfg) {
    pb2_plugin->register_event_handler<pb2::event_privmsg>([] (pb2::event::ptr _e) {
      pb2::event_privmsg::ptr e = pb2_ptrcast<pb2::event_privmsg>(_e);
      
      static std::regex url_regex(".*?((?:(?:https?):\\/\\/)(?:\\S+(?::\\S*)?@)?(?:(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))|(?:(?:[a-z\\u00a1-\\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\\.(?:[a-z\u00a1-\uffff]{2,}))\\.?)(?::\\d{2,5})?(?:[\\/?#]\\S*)?).*");
      static std::regex bot_regex(".*bot.*");
      
      std::smatch m {};
      std::smatch _m {};
      
      std::string b = e->nick + "!" + e->user + "@" + e->host;
      if (std::regex_match(e->message, m, url_regex) && !std::regex_match(b, _m, bot_regex)) {
        std::string url = m[1];
        particledi::dm_ptr dm = e->pbot->get_dm();
        std::shared_ptr<pb2::db_service> db_s = dm->get<pb2::db_service>();
        pb2::flag f(e->socket->get_name());
        f.channel = e->target;
        f.host    = e->host;
        f.plugin  = pb2_plugin->name;
        f.name    = "enable";
        bool enable_titler = db_s->check(f);
        if (enable_titler) {
          try {
            std::string ret;
            if (!send(url, ret)) {
              return;
            }
            xmlDoc* doc = htmlReadDoc((xmlChar*) ret.c_str(), NULL, "utf-8", HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
          
            xmlNode* r = xmlDocGetRootElement(doc);
            xmlpp::Element* root = new xmlpp::Element(r);
          
            try {
              std::string xpath = "//title/text()";
              auto elements = root->find(xpath);
              if (elements.size() > 0) {
                auto n = static_cast<xmlpp::ContentNode*>(elements[0]);
                std::string title = n->get_content().substr(0, 256);
                e->socket->stream() << pb2::ircstream::reply(e, u8"%C?GREEN&B^&N " + title);
              } else {
                e->socket->stream() << pb2::ircstream::reply(e, u8"%C?BLUE&B^&N <no title>");
              }
            } catch (std::exception& exc) {
              e->socket->stream() << pb2::ircstream::reply(e, u8"%C?RED&B^&N <error>");
              delete root;
              xmlFreeDoc(doc);
              throw exc;
            }
            delete root;
            xmlFreeDoc(doc);
          } catch (std::exception& exc) {
            e->socket->stream() << pb2::ircstream::reply(e, u8"%C?RED&B^&N <error>");
            throw exc;
          }
        }
      }
      
    });
  }
  
  void pb2_deinit() {}
}
