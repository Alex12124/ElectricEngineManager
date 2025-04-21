#pragma once

#include <QFuture>
#include <memory>

#include "boost/asio/io_context.hpp"
#include "message.pb.h"
#include "session.h"

namespace server {

class Server {
 public:
  Server();
  ~Server();
  void Run();
  void Stop();
  bool IsSessionAlive();
  std::optional<message::Request> TryGetRequest();
  void SendResponse(const message::Response&);

 private:
  boost::asio::io_context io_context_;
  std::shared_ptr<Session> active_session_;
  QFuture<void> run_server_future_;
};

}  // namespace server
