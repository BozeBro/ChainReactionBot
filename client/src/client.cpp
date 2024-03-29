
#ifdef SECURE
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include "httplib.h"

#include "websocketpp/common/connection_hdl.hpp"
#include "websocketpp/connection.hpp"
#include "websocketpp/frame.hpp"
#include "websocketpp/logger/levels.hpp"
#include <client.hpp>
#include <httplib.h>
#include <ostream>
#include <sstream>
#include <string>
#ifdef SECURE
static constexpr char HTTP[] = "https";
static constexpr char WS[] = "wss";
#else
static constexpr char HTTP[] = "http";
static constexpr char WS[] = "ws";
#endif

namespace {
std::string http_from_host(std::string host) noexcept {
  std::stringstream httpUri;
  httpUri << HTTP << "://" << host;
  return httpUri.str();
}
} // namespace
using context_ptr = std::shared_ptr<boost::asio::ssl::context>;
// pull out the type of messages sent by our config
// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void BotClient::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
  std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload() << std::endl;
  json data = json::parse(msg->get_payload());

  websocketpp::lib::error_code ec;
  //   std::cout << websocketpp::frame::opcode::text << '\n';

  //   if (ec) {
  //     std::cout << "Echo failed because: " << ec.message() << std::endl;
  //   }
  Payload payload = m_callback(data);
  if (payload.is_send)
    m_webSocket->send(hdl, payload.m_payload.dump(),
                      websocketpp::frame::opcode::text);
}
// TODO: Look later about how this tls init actually works
static context_ptr on_tls_init() {
  // establishes a SSL connection
  context_ptr ctx = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::sslv23);

  try {
    ctx->set_options(boost::asio::ssl::context::default_workarounds |
                     boost::asio::ssl::context::no_sslv2 |
                     boost::asio::ssl::context::no_sslv3 |
                     boost::asio::ssl::context::single_dh_use);
  } catch (std::exception &e) {
    std::cout << "Error in context pointer: " << e.what() << std::endl;
  }
  return ctx;
}
BotClient::BotClient(std::string host, std::string id,
                     std::function<Payload(json)> callback)
    : m_host(host), m_id(id),
      m_client(std::make_unique<httplib::Client>(http_from_host(host))),
      m_webSocket(std::make_unique<client>()), m_callback(callback) {
  std::cout << "Hello World\n";
  // Set logging to be pretty verbose (everything except message payloads)
  m_webSocket->set_access_channels(websocketpp::log::alevel::all);
  m_webSocket->clear_access_channels(websocketpp::log::alevel::frame_payload);

  // Initialize ASIO
  m_webSocket->init_asio();
#ifdef PROD
  m_webSocket.set_tls_init_handler(bind(&on_tls_init));
#endif

  // Register our message handler
  auto message_handler = [this](websocketpp::connection_hdl hdl,
                                message_ptr msg) {
    this->on_message(hdl, msg);
  };
  m_webSocket->set_message_handler(message_handler);
  m_webSocket->set_fail_handler([this](websocketpp::connection_hdl hdl) {
    auto con = m_webSocket->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_ec() << " ("
      << std::to_string(con->get_remote_close_code())
      << "), close reason: " << con->get_local_close_reason();
    std::cout << s.str();
  });
}

std::string BotClient::connect() {
  std::stringstream httpPath;
  httpPath << "/game/" << m_id;
  httplib::Result res = m_client->Get(httpPath.str());
  if (res.error() != httplib::Error::Success) {
    std::stringstream err;
    err << "Error from http: " << httplib::to_string(res.error());
    return err.str();
  }
  std::stringstream wsPath;
  wsPath << WS << "://" << m_host << "/ws/" << m_id;
  std::cout << "PATH IS : " << wsPath.str() << std::endl;

  websocketpp::lib::error_code ec;
  client::connection_ptr con = m_webSocket->get_connection(wsPath.str(), ec);
  if (ec) {
    std::stringstream err;
    err << "Coult not create connection because: " << ec.message() << std::endl;
    return err.str();
  }
  // Note that connect here only requests a connection. No network messages are
  // exchanged until the event loop starts running in the next line.
  m_webSocket->connect(con);

  // Start the ASIO io_service run loop
  // this will cause a single connection to be made to the server. c.run()
  // will exit when this connection is closed.
  m_webSocket->run();
  return "";
}
BotClient::~BotClient() {}
