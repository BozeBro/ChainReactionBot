#pragma once
#include "httplib.h"
#include "websocketpp/common/connection_hdl.hpp"
#include <string>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include <boost/format.hpp>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;
#ifdef PROD
using client = websocketpp::client<websocketpp::config::asio_tls_client>;
#endif // PROD
#ifdef DEBUG
using client = websocketpp::client<websocketpp::config::asio_client>;
#endif // DEBUG

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg);

class BotClient {
public:
  BotClient(string host, string id) noexcept;
  inline void set_id(string id) { m_id = id; }
  string connect();

protected:
  void init_ws();

private:
  string m_host;
  string m_id;
  httplib::Client m_client;
  client m_webSocket;
  client::connection_ptr m_con;
};
