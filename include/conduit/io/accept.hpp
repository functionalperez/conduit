#pragma once
#include <conduit/async/callback.hpp>
#include <conduit/mixin/resumable.hpp>
#include <conduit/coroutine.hpp>

#include <conduit/io/responses.hpp>

#include <boost/asio/basic_socket.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

namespace conduit::async {
namespace asio = boost::asio;

template<class Protocol, class Executor>
class accept : public mixin::Resumable<accept<Protocol, Executor>> {
    using acceptor_type = boost::asio::basic_socket_acceptor<Protocol, Executor>;
    using socket_type = boost::asio::basic_socket<Protocol>;
    acceptor_type& acceptor;
    socket_type& socket;
    error_code const* status = nullptr;
    auto get_handler(std::coroutine_handle<> h) {
        return [this, caller = async::callback{h}](
                   error_code const& response) mutable {
            status = &response;
            caller.resume();
        };
    }
    void on_suspend(std::coroutine_handle<> h) {
        acceptor.async_accept(socket, get_handler(h));
    }
    friend class mixin::Resumable<accept>;

   public:
    accept() = default;
    accept(accept const&) = default;
    accept(acceptor_type& acceptor, socket_type& socket)
      : acceptor(acceptor), socket(socket) {}

    status_result await_resume() noexcept { return {*status}; }
};
} // namespace conduit::async
