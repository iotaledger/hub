/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

/// The following implementation is based on:
/// https://www.boost.org/doc/libs/develop/libs/beast/example/advanced/server/advanced_server.cpp

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include <glog/logging.h>

#include <boost/make_unique.hpp>
#include <boost/optional.hpp>

#include "boost/asio/ssl/verify_mode.hpp"
#include "common/common.h"
#include "common/flags.h"
#include "common/http_server_base.h"

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;             // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;        // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

namespace common {
// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template <class Body, class Allocator, class Send>
void handleRequest(HttpServerBase& server,
                   http::request<Body, http::basic_fields<Allocator>>&& req,
                   Send&& send) {
  // Returns a bad request response
  auto const badRequest = [&req](beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  // Returns a server error response
  auto const serverError = [&req](beast::string_view what) {
    http::response<http::string_body> res{http::status::internal_server_error,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'";
    res.prepare_payload();
    return res;
  };

  // Make sure we can handle the method
  if (req.method() != http::verb::post) {
    return send(badRequest("Unknown HTTP-method"));
  }

  if (req.body().empty()) {
    return send(serverError("Empty request"));
  }

  std::string responseBody;
  auto ec = server.handleRequestImpl(req.body(), responseBody);
  if (ec != HttpServerBase::OK) {
    return send(badRequest(responseBody));
  }

  http::response<http::string_body> res{http::status::ok, req.version()};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, "text/html");
  res.keep_alive(req.keep_alive());
  res.body() = responseBody;
  res.prepare_payload();
  return send(std::move(res));
}
//------------------------------------------------------------------------------

// Report a failure
void httpFail(beast::error_code ec, char const* what) {
  // ssl::error::stream_truncated, also known as an SSL "short read",
  // indicates the peer closed the connection without performing the
  // required closing handshake (for example, Google does this to
  // improve performance). Generally this can be a security issue,
  // but if your communication protocol is self-terminated (as
  // it is with both HTTP and WebSocket) then you may simply
  // ignore the lack of close_notify.
  //
  // https://github.com/boostorg/beast/issues/38
  //
  // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
  //
  // When a short read would cut off the end of an HTTP message,
  // Beast returns the error beast::http::error::partial_message.
  // Therefore, if we see a short read here, it has occurred
  // after the message has been completed, so it is safe to ignore it.

  if (ec == net::ssl::error::stream_truncated) return;

  LOG(ERROR) << what << ": " << ec.message();
}

//------------------------------------------------------------------------------

// Echoes back all received WebSocket messages.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived>
class WebsocketSession {
  // Access the derived class, this is part of
  // the Curiously Recurring Template Pattern idiom.
  Derived& derived() { return static_cast<Derived&>(*this); }

  beast::flat_buffer _buffer;

  // Start the asynchronous operation
  template <class Body, class Allocator>
  void doAccept(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    derived().ws().set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    derived().ws().set_option(
        websocket::stream_base::decorator([](websocket::response_type& res) {
          res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                           " advanced-server-flex");
        }));

    // Accept the websocket handshake
    derived().ws().async_accept(
        req, beast::bind_front_handler(&WebsocketSession::onAccept,
                                       derived().shared_from_this()));
  }

  void onAccept(beast::error_code ec) {
    if (ec) return httpFail(ec, "accept");

    // Read a message
    doRead();
  }

  void doRead() {
    // Read a message into our buffer
    derived().ws().async_read(
        _buffer, beast::bind_front_handler(&WebsocketSession::onRead,
                                           derived().shared_from_this()));
  }

  void onRead(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the websocket_session was closed
    if (ec == websocket::error::closed) return;

    if (ec) httpFail(ec, "read");

    // Echo the message
    derived().ws().text(derived().ws().got_text());
    derived().ws().async_write(
        _buffer.data(),
        beast::bind_front_handler(&WebsocketSession::onWrite,
                                  derived().shared_from_this()));
  }

  void onWrite(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) return httpFail(ec, "write");

    // Clear the buffer
    _buffer.consume(_buffer.size());

    // Do another read
    doRead();
  }

 public:
  // Start the asynchronous operation
  template <class Body, class Allocator>
  void run(http::request<Body, http::basic_fields<Allocator>> req) {
    // Accept the WebSocket upgrade request
    doAccept(std::move(req));
  }
};
//------------------------------------------------------------------------------

// Handles a plain WebSocket connection
class PlainWebsocketSession
    : public WebsocketSession<PlainWebsocketSession>,
      public std::enable_shared_from_this<PlainWebsocketSession> {
  websocket::stream<beast::tcp_stream> _ws;

 public:
  // Create the session
  explicit PlainWebsocketSession(beast::tcp_stream&& stream)
      : _ws(std::move(stream)) {}

  // Called by the base class
  websocket::stream<beast::tcp_stream>& ws() { return _ws; }
};

//------------------------------------------------------------------------------

// Handles an SSL WebSocket connection
class SslWebsocketSession
    : public WebsocketSession<SslWebsocketSession>,
      public std::enable_shared_from_this<SslWebsocketSession> {
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> _ws;

 public:
  // Create the SslWebsocketSession
  explicit SslWebsocketSession(beast::ssl_stream<beast::tcp_stream>&& stream)
      : _ws(std::move(stream)) {}

  // Called by the base class
  websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws() { return _ws; }
};
//------------------------------------------------------------------------------

template <class Body, class Allocator>
void make_websocket_session(
    beast::tcp_stream stream,
    http::request<Body, http::basic_fields<Allocator>> req) {
  std::make_shared<PlainWebsocketSession>(std::move(stream))
      ->run(std::move(req));
}

template <class Body, class Allocator>
void make_websocket_session(
    beast::ssl_stream<beast::tcp_stream> stream,
    http::request<Body, http::basic_fields<Allocator>> req) {
  std::make_shared<SslWebsocketSession>(std::move(stream))->run(std::move(req));
}
//------------------------------------------------------------------------------

// Handles an HTTP server connection.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template <class Derived>
class httpSession {
  // Access the derived class, this is part of
  // the Curiously Recurring Template Pattern idiom.
  Derived& derived() { return static_cast<Derived&>(*this); }

  // This queue is used for HTTP pipelining.
  class Queue {
    enum {
      // Maximum number of responses we will queue
      limit = 8
    };

    // The type-erased, saved work item
    struct Work {
      virtual ~Work() = default;
      virtual void operator()() = 0;
    };

    httpSession& _self;
    std::vector<std::unique_ptr<Work>> _items;

   public:
    explicit Queue(httpSession& self) : _self(self) {
      static_assert(limit > 0, "queue limit must be positive");
      _items.reserve(limit);
    }

    // Returns `true` if we have reached the queue limit
    bool isFull() const { return _items.size() >= limit; }

    // Called when a message finishes sending
    // Returns `true` if the caller should initiate a read
    bool onWrite() {
      BOOST_ASSERT(!_items.empty());
      auto const wasFull = isFull();
      _items.erase(_items.begin());
      if (!_items.empty()) (*_items.front())();
      return wasFull;
    }

    // Called by the HTTP handler to send a response.
    template <bool isRequest, class Body, class Fields>
    void operator()(http::message<isRequest, Body, Fields>&& msg) {
      // This holds a work item
      struct WorkImpl : Work {
        httpSession& _self;
        http::message<isRequest, Body, Fields> _msg;

        WorkImpl(httpSession& self,
                 http::message<isRequest, Body, Fields>&& msg)
            : _self(self), _msg(std::move(msg)) {}

        void operator()() {
          http::async_write(
              _self.derived().stream(), _msg,
              beast::bind_front_handler(&httpSession::onWrite,
                                        _self.derived().shared_from_this(),
                                        _msg.need_eof()));
        }
      };

      // Allocate and store the work
      _items.push_back(boost::make_unique<WorkImpl>(_self, std::move(msg)));

      // If there was no previous work, start this one
      if (_items.size() == 1) (*_items.front())();
    }
  };

  HttpServerBase& _server;
  Queue _queue;

  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> _parser;

 protected:
  beast::flat_buffer _buffer{8192};

 public:
  // Construct the session
  httpSession(HttpServerBase& server, beast::flat_buffer buffer)
      : _server(server), _queue(*this), _buffer(std::move(buffer)) {}

  void doRead() {
    // Construct a new parser for each message
    _parser.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    _parser->body_limit(10000);

    // Set the timeout.
    beast::get_lowest_layer(derived().stream())
        .expires_after(std::chrono::seconds(30));
    // Read a request using the parser-oriented interface
    http::async_read(derived().stream(), _buffer, *_parser,
                     beast::bind_front_handler(&httpSession::onRead,
                                               derived().shared_from_this()));
  }

  void onRead(beast::error_code ec, std::size_t bytesTransferred) {
    boost::ignore_unused(bytesTransferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream) return derived().doEof();

    if (ec) return httpFail(ec, "read");

    // See if it is a WebSocket Upgrade
    if (websocket::is_upgrade(_parser->get())) {
      // Disable the timeout.
      // The websocket::stream uses its own timeout settings.
      beast::get_lowest_layer(derived().stream()).expires_never();

      // Create a websocket session, transferring ownership
      // of both the socket and the HTTP request.
      return make_websocket_session(derived().releaseStream(),
                                    _parser->release());
    }

    // Send the response
    handleRequest(_server, _parser->release(), _queue);

    // If we aren't at the queue limit, try to pipeline another request
    if (!_queue.isFull()) doRead();
  }

  void onWrite(bool close, beast::error_code ec,
               std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) return httpFail(ec, "write");

    if (close) {
      // This means we should close the connection, usually because
      // the response indicated the "Connection: close" semantic.
      return derived().doEof();
    }

    // Inform the queue that a write completed
    if (_queue.onWrite()) {
      // Read another request
      doRead();
    }
  }
};

//----

//------------------------------------------------------------------------------

// Handles a plain HTTP connection
class PlainHttpSession : public httpSession<PlainHttpSession>,
                         public std::enable_shared_from_this<PlainHttpSession> {
  beast::tcp_stream _stream;

 public:
  // Create the session
  PlainHttpSession(HttpServerBase& server, beast::tcp_stream&& stream,
                   beast::flat_buffer&& buffer)
      : httpSession<PlainHttpSession>(server, std::move(buffer)),
        _stream(std::move(stream)) {}

  // Start the session
  void run() { this->doRead(); }

  // Called by the base class
  beast::tcp_stream& stream() { return _stream; }

  // Called by the base class
  beast::tcp_stream releaseStream() { return std::move(_stream); }

  // Called by the base class
  void doEof() {
    // Send a TCP shutdown
    beast::error_code ec;
    _stream.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

// Handles an SSL HTTP connection
class SslHttpSession : public httpSession<SslHttpSession>,
                       public std::enable_shared_from_this<SslHttpSession> {
  beast::ssl_stream<beast::tcp_stream> _stream;

 public:
  // Create the http_session
  SslHttpSession(HttpServerBase& server, beast::tcp_stream&& stream,
                 ssl::context& ctx, beast::flat_buffer&& buffer)
      : httpSession<SslHttpSession>(server, std::move(buffer)),
        _stream(std::move(stream), ctx) {}

  // Start the session
  void run() {
    // Set the timeout.
    beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(30));

    // Perform the SSL handshake
    // Note, this is the buffered version of the handshake.
    _stream.async_handshake(
        ssl::stream_base::server, _buffer.data(),
        beast::bind_front_handler(&SslHttpSession::onHandshake,
                                  shared_from_this()));
  }

  // Called by the base class
  beast::ssl_stream<beast::tcp_stream>& stream() { return _stream; }

  // Called by the base class
  beast::ssl_stream<beast::tcp_stream> releaseStream() {
    return std::move(_stream);
  }

  // Called by the base class
  void doEof() {
    // Set the timeout.
    beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(30));

    // Perform the SSL shutdown
    _stream.async_shutdown(beast::bind_front_handler(
        &SslHttpSession::onShutdown, shared_from_this()));
  }

 private:
  void onHandshake(beast::error_code ec, std::size_t bytesUsed) {
    if (ec) return httpFail(ec, "handshake");

    // Consume the portion of the buffer used by the handshake
    _buffer.consume(bytesUsed);

    doRead();
  }

  void onShutdown(beast::error_code ec) {
    if (ec) return httpFail(ec, "shutdown");

    // At this point the connection is closed gracefully
  }
};

// Detects SSL handshakes
class HttpDetectSession
    : public std::enable_shared_from_this<HttpDetectSession> {
  HttpServerBase& _server;
  beast::tcp_stream _stream;
  ssl::context& _ctx;
  beast::flat_buffer _buffer;

 public:
  explicit HttpDetectSession(HttpServerBase& server, tcp::socket&& socket,
                             ssl::context& ctx)
      : _server(server), _stream(std::move(socket)), _ctx(ctx) {}

  // Launch the detector
  void run() {
    // Set the timeout.
    _stream.expires_after(std::chrono::seconds(30));

    beast::async_detect_ssl(
        _stream, _buffer,
        beast::bind_front_handler(&HttpDetectSession::onDetect,
                                  this->shared_from_this()));
  }

  void onDetect(beast::error_code ec, boost::tribool result) {
    if (ec) return httpFail(ec, "detect");

    if (result) {
      // Launch SSL session
      std::make_shared<SslHttpSession>(_server, std::move(_stream), _ctx,
                                       std::move(_buffer))
          ->run();
      return;
    }

    // Launch plain session
    std::make_shared<PlainHttpSession>(_server, std::move(_stream),
                                       std::move(_buffer))
        ->run();
  }
};

// Accepts incoming connections and launches the sessions
class HttpListener : public std::enable_shared_from_this<HttpListener> {
  HttpServerBase& _server;
  net::io_context& _ioc;
  ssl::context& _ctx;
  tcp::acceptor _acceptor;

 public:
  HttpListener(HttpServerBase& server, net::io_context& ioc, ssl::context& ctx,
               tcp::endpoint endpoint)
      : _server(server),
        _ioc(ioc),
        _ctx(ctx),
        _acceptor(net::make_strand(ioc)) {
    beast::error_code ec;

    // Open the acceptor
    _acceptor.open(endpoint.protocol(), ec);
    if (ec) {
      httpFail(ec, "open");
      return;
    }

    // Allow address reuse
    _acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
      httpFail(ec, "set_option");
      return;
    }

    // Bind to the server address
    _acceptor.bind(endpoint, ec);
    if (ec) {
      httpFail(ec, "bind");
      return;
    }

    // Start listening for connections
    _acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
      httpFail(ec, "listen");
      return;
    }
  }

  // Start accepting incoming connections
  void run() { doAccept(); }

 private:
  void doAccept() {
    // The new connection gets its own strand
    _acceptor.async_accept(
        net::make_strand(_ioc),
        beast::bind_front_handler(&HttpListener::onAccept, shared_from_this()));
  }

  void onAccept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
      httpFail(ec, "accept");
    } else {
      // Create the detector http_session and run it
      std::make_shared<HttpDetectSession>(_server, std::move(socket), _ctx)
          ->run();
    }

    // Accept another connection
    doAccept();
  }
};

void HttpServerBase::setupCredentials(ssl::context& ctx) {
  // This holds the self-signed certificate used by the server

  boost::system::error_code ec;

  if (common::flags::FLAGS_authMode == "none") {
    return;
  } else if (common::flags::FLAGS_authMode == "ssl") {
    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

    ctx.set_verify_mode(ssl::verify_peer);

    ctx.use_certificate_chain_file(common::flags::FLAGS_sslCert, ec);
    if (ec.failed()) {
      LOG(FATAL) << "Failed setting certificate chain file with error: "
                 << ec.message();
    }
    ctx.use_private_key_file(common::flags::FLAGS_sslKey,
                             boost::asio::ssl::context::file_format::pem, ec);
    if (ec.failed()) {
      LOG(FATAL) << "Failed setting private key file with error: "
                 << ec.message();
    }
    ctx.use_tmp_dh_file(common::flags::FLAGS_sslDH, ec);
    if (ec.failed()) {
      LOG(FATAL) << "Failed setting DH file with error: " << ec.message();
    }
  } else {
    LOG(ERROR) << "Unknown auth mode: " << common::flags::FLAGS_authMode;
  }
}

void HttpServerBase::runAndWait() {
  auto listenAddress = common::flags::FLAGS_listenAddress;
  try {
    auto portPos = listenAddress.find(':');
    if (portPos == std::string::npos) {
      LOG(ERROR) << "Wrong listen address value: " << listenAddress;
    }
    std::string addressStr = listenAddress.substr(0, portPos);
    auto const address = net::ip::make_address(addressStr);
    unsigned short port = static_cast<unsigned short>(std::atoi(
        listenAddress.substr(portPos + 1, listenAddress.size()).c_str()));

    auto const threads = std::max<int>(1, common::flags::FLAGS_numThreadsHttp);

    // The io_context is required for all I/O
    net::io_context ioc{threads};

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12};

    HttpServerBase::setupCredentials(ctx);

    // Create and launch a listening port
    std::make_shared<HttpListener>(*this, ioc, ctx,
                                   tcp::endpoint{address, port})
        ->run();

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&](beast::error_code const&, int) {
      // Stop the `io_context`. This will cause `run()`
      // to return immediately, eventually destroying the
      // `io_context` and all of the sockets in it.
      ioc.stop();
    });

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
      v.emplace_back([&ioc] { ioc.run(); });
    ioc.run();

    // (If we get here, it means we got a SIGINT or SIGTERM)

    // Block until all the threads exit
    for (auto& t : v) t.join();
  } catch (const std::exception& e) {
    LOG(ERROR) << "Failed initializing http server";
  }
}

}  // namespace common
