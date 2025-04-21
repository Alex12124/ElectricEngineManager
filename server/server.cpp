#include "server.h"

#include <QtConcurrent>
#include <boost/asio/detached.hpp>

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/spawn.hpp"
#include "session.h"

using boost::asio::ip::tcp;
namespace server {

std::optional<message::Request> Server::TryGetRequest() {
  return active_session_->TryGetRequest();
}

void Server::SendResponse(const message::Response& response) {
  return active_session_->SendResponse(response);
}

bool Server::IsSessionAlive() {
  return active_session_.get() != nullptr && active_session_->IsAlive();
}

void Server::Stop() {}

Server::Server() {}

Server::~Server() {
  io_context_.stop();
  run_server_future_.waitForFinished();
}

void Server::Run() {
  run_server_future_ =
      QtConcurrent::run(QThreadPool::globalInstance(), [this]() {
        boost::asio::spawn(
            io_context_,
            [&](boost::asio::yield_context yield) {
              tcp::acceptor acceptor(io_context_,
                                     tcp::endpoint(tcp::v4(), 60000));
              while (true) {
                boost::system::error_code ec;
                tcp::socket socket(io_context_);
                acceptor.async_accept(socket, yield[ec]);
                if (!ec) {
                  if (!IsSessionAlive()) {
                    active_session_ = std::make_shared<Session>(
                        io_context_, std::move(socket));
                    active_session_->Start();
                  } else {
                    socket.close();
                  }
                }
              }
            },
            boost::asio::detached);

        io_context_.run();
      });
}

}  // namespace server
