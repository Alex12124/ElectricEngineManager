#include "session.h"

#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <memory>
#include <mutex>

#include "message.pb.h"
using boost::asio::ip::tcp;

namespace server {

Session::Session(boost::asio::io_context& io_context, tcp::socket socket)
    : socket_(std::move(socket)), io_context_(io_context) {}

Session::~Session() {}

void Session::CloseResources() { socket_.close(); }

std::optional<message::Request> Session::ReadRequest(
    boost::asio::yield_context& yield) {
  std::string data;
  boost::system::error_code read_ec;
  uint32_t msg_size;
  boost::asio::async_read(socket_,
                          boost::asio::buffer(&msg_size, sizeof(msg_size)),
                          yield[read_ec]);
  msg_size = ntohl(msg_size);
  if (read_ec) {
    return std::nullopt;
  }
  data.resize(msg_size);
  boost::asio::async_read(socket_, boost::asio::buffer(data.data(), msg_size),
                          yield[read_ec]);
  if (read_ec) {
    return std::nullopt;
  }
  message::Request req;
  if (req.ParseFromString(data)) {
    return req;
  }
  return std::nullopt;
}

void Session::Start() { OnRequest(); }

bool Session::IsAlive() {
  std::lock_guard lock(mutex_);
  return socket_.is_open();
  ;
}

std::optional<message::Request> Session::TryGetRequest() {
  return requests_.TryPop();
}

void Session::SendResponse(const message::Response& msg) {
  bool res = responses_.TryPush(msg);
  if (res) {
    OnResponse();
  } else {
    auto self = shared_from_this();
    boost::asio::post(io_context_, [self, this]() { CloseResources(); });
  }
}

void Session::OnRequest() {
  auto self = shared_from_this();
  boost::asio::spawn(
      io_context_,
      [self, this](boost::asio::yield_context yield) {
        while (true) {
          std::optional<message::Request> maybe_request = ReadRequest(yield);
          if (!maybe_request.has_value()) {
            CloseResources();
            return;
          }
          bool push_success = requests_.TryPush(maybe_request.value());
          if (!push_success) {
            CloseResources();
            return;
          }
        }
      },
      boost::asio::detached);
}

void Session::OnResponse() {
  auto self = shared_from_this();
  boost::asio::spawn(
      io_context_,
      [self, this](boost::asio::yield_context yield) {
        boost::system::error_code write_ec;
        std::optional<message::Response> maybe_response = responses_.TryPop();
        if (!maybe_response.has_value()) {
          CloseResources();
          return;
        }
        std::string response_str = maybe_response->SerializeAsString();
        uint32_t msg_size = htonl(response_str.size());

        async_write(socket_, boost::asio::buffer(&msg_size, sizeof(msg_size)),
                    yield[write_ec]);

        if (write_ec) {
          CloseResources();
          return;
        }

        async_write(socket_, boost::asio::buffer(response_str),
                    yield[write_ec]);
        if (write_ec) {
          CloseResources();
          return;
        }
      },
      boost::asio::detached);
}

}  // namespace server
