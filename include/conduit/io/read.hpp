#pragma once
#include <conduit/future.hpp>
#include <conduit/async/callback.hpp>

#include <conduit/io/responses.hpp>

#include <boost/asio/basic_socket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>

#include <array>
#include <coroutine>
#include <span>
#include <string>
#include <string_view>

namespace conduit::io {
using boost::asio::ip::tcp;
namespace asio = boost::asio;

template <class Protocol>
class read_some {
    using socket_type = boost::asio::basic_stream_socket<Protocol>;
    template <size_t N>
    using char_buff = char[N];

    socket_type& socket = nullptr;
    error_code const* status = nullptr;

    std::span<char> buffer;

    auto get_handler(std::coroutine_handle<> h) {
        return [this, caller = async::callback(h)](error_code const& response,
                                                   size_t s) mutable {
            status = &response;
            buffer = buffer.subspan(0, s);
            caller.resume();
        };
    }

   public:
    read_some() = default;
    read_some(read_some const&) = default;
    read_some(socket_type& socket, std::span<char> buffer) noexcept
      : socket(socket), buffer(buffer) {}

    constexpr bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        socket.async_read_some(
            asio::mutable_buffer(buffer.data(), buffer.size()), get_handler(h));
    }
    partial_read_result await_resume() {
        return {*status, std::string_view(buffer.data(), buffer.size())};
    }
};

template <class Protocol>
future<read_result> read(asio::basic_stream_socket<Protocol>& socket) {
    std::array<char, 1024> buffer;
    std::string result;
    while (true) {
        auto response = co_await read_some{socket, buffer};

        if(!response) {
            co_return read_result{response.status(), result};
        } 

        result += response;
    }
}
} // namespace conduit::async
