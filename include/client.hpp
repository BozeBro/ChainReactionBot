#pragma once

#include "httplib.h"

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
namespace httplib {
class Client;
}

class BotClient {
public:
  BotClient(string host, string id) noexcept;
  BotClient(const BotClient &) = delete;
  BotClient &operator=(const BotClient &) = delete;
  BotClient(BotClient &&) = default;
  inline void set_id(string id) { m_id = id; }
  string connect();

protected:
  void init_ws();

private:
  string m_host;
  string m_id;
  httplib::Client m_client;
  client m_webSocket;
};
