#include "db_service.hpp"

namespace pb2 {

  /*
   * Flag implementation
   */

  flag::flag(std::string _server)
  : server(_server)
  {}
  
  /*
   * Private declaration
   */
  
  class db_service_private {
  public:
    db_service_private(db_service& pub, std::string& path);
    
    bool check(flag& f);
    void insert(flag& f);
    void remove(flag& f);
    
    db_service& pub;
  };
  
  /*
   * Private implementation
   */
  
  db_service_private::db_service_private(db_service& _pub, std::string& path)
  : pub(_pub)
  {}
  
  bool db_service_private::check(flag& f) {
    return true;
  }
  
  void db_service_private::insert(flag& f) {
    
  }
  
  void db_service_private::remove(flag& f) {
    
  }
  
  /*
   * Public implementation
   */

  db_service::db_service(std::string path)
  : priv(new db_service_private(*this, path))
  {}
  
  
  db_service::~db_service() {}

  bool db_service::check(flag& f) {
    return priv->check(f);
  }
  
  void db_service::insert(flag& f) {
    priv->insert(f);
  }
  
  void db_service::remove(flag& f) {
    priv->remove(f);
  }

}