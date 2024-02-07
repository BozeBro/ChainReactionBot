#pragma once

#include <string>
#include <websocketpp/client.hpp>

#include <boost/format.hpp>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

namespace httplib {
class Client;
}

#ifdef SECURE
using client = websocketpp::client<websocketpp::config::asio_tls_client>;
#else
using client = websocketpp::client<websocketpp::config::asio_client>;
#endif

using json = nlohmann::json;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;

struct Payload {
  json m_payload;
  bool is_send;
};

class BotClient {
public:
  // callback arg should contain information from server
  BotClient(std::string host, std::string id,
            std::function<Payload(json)> callback);
  BotClient(const BotClient &) = delete;
  void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
  BotClient &operator=(const BotClient &) = delete;
  BotClient(BotClient &&) = default;
  void set_id(std::string id) { m_id = id; }
  // entrypoint
  std::string connect();
  ~BotClient();

private:
  std::string m_host;
  std::string m_id;
  std::unique_ptr<httplib::Client> m_client;
  std::unique_ptr<client> m_webSocket;
  std::function<Payload(json)> m_callback;
};
