#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <mutex>
#include <optional>

#include "message.pb.h"
#include "synchronized_queue.h"

namespace server {

class Session : public std::enable_shared_from_this<Session> {
 private:
  boost::asio::ip::tcp::socket socket_;
  boost::asio::io_context& io_context_;
  std::mutex mutex_;

  SynchronizedQueue<message::Request, 100> requests_;
  SynchronizedQueue<message::Response, 100> responses_;

 private:
  void CloseResources();

  // void OnResponse(const QString&);

 public:
  Session(boost::asio::io_context& io_context,
          boost::asio::ip::tcp::socket socket);
  ~Session();

  std::optional<message::Request> ReadRequest(
      boost::asio::yield_context& yield);
  void Start();

  bool IsAlive();  // Guarded by mutex
  std::optional<message::Request> TryGetRequest();
  void SendResponse(const message::Response&);
  void OnRequest();
  void OnResponse();
};

}  // namespace server
