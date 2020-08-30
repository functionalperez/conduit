#pragma once
#include <conduit/io/responses.hpp>
#include <conduit/async/callback.hpp>

#include <boost/asio/basic_socket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>

#include <coroutine>
#include <string>

namespace conduit::io {
using boost::asio::ip::tcp;
namespace asio = boost::asio;

template <class Socket>
class write {
    std::string_view message;
    Socket& socket;
    error_code const* status;

    auto get_handler(std::coroutine_handle<> h) {
        return [this, caller = async::callback(h)](error_code const& e,
                                                   size_t s) mutable {
            status = &e;
            message = message.substr(0, s);
            caller.resume();
        };
    }

   public:
    write() = default;
    write(write const&) = default;
    write(write&&) = default;
    write(Socket& socket, std::string_view data)
      : socket(socket), message(data) {}

    constexpr bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        asio::async_write(socket, asio::buffer(message), get_handler(h));
    }
    write_result await_resume() { return {*status, message.size()}; }
}; // namespace conduit::async
template <class Socket>
write(Socket& s, std::string_view data) -> write<Socket>;

template<class Socket>
coroutine start_write(Socket socket, std::string message) {
    co_await write(socket, message);
}
} // namespace conduit::async
