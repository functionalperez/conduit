#pragma once
#include <conduit/mixin/resumable.hpp>

#include <conduit/io/responses.hpp>

#include <boost/asio/basic_socket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <coroutine>

namespace conduit::io {
using boost::asio::ip::tcp;
using boost::system::error_code;

template <class... Args>
class connect;

template <class Protocol, class EndpointSequence>
struct connect<Protocol, EndpointSequence>
  : mixin::Resumable<connect<Protocol, EndpointSequence>> {
    using endpoint = typename Protocol::endpoint;

    boost::asio::basic_socket<Protocol>& socket;
    EndpointSequence const& seq;
    error_code const* ec_pointer;
    endpoint const* ep_pointer;
    auto get_handler(std::coroutine_handle<> h) {
        return [this, caller = async::callback(h)](error_code const& ec,
                                            endpoint const& ep) mutable {
            ec_pointer = &ec;
            ep_pointer = &ep;
            caller.resume();
        };
    }
    void on_suspend(std::coroutine_handle<> h) {
        boost::asio::async_connect(socket, seq, get_handler(h));
    }

    connect(boost::asio::basic_socket<Protocol>& socket,
            EndpointSequence const& seq) noexcept
      : socket(socket), seq(seq) {}

    auto await_resume() noexcept {
        return connect_result{*ec_pointer, *ep_pointer};
    }
};
template<class Protocol, class EndpointSequence>
connect(boost::asio::basic_socket<Protocol>& socket, EndpointSequence const& seq) -> connect<Protocol, EndpointSequence>;
} // namespace conduit::async
