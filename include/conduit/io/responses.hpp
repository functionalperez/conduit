#pragma once
#include <concepts>
#include <string_view>
#include <type_traits>

#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>

namespace conduit::io {
using boost::system::error_code;
struct status_result : error_code {
    status_result() = default;
    status_result(error_code const& err) : error_code(err) {}
    inline error_code status() const { return *this; }
    inline bool good() const { return !error_code::operator bool(); }
    inline bool bad() const { return error_code::operator bool(); }
    inline bool eof() const { return status() == boost::asio::error::eof; }
    inline operator bool() const { return good(); }
    using error_code::category;
    using error_code::message;
    using error_code::value;
};

template <class Endpoint>
struct connect_result : status_result {
    Endpoint const& endpoint;
};
template <class Endpoint>
connect_result(status_result, Endpoint const&) -> connect_result<Endpoint>;

template <class EndpointList>
struct resolve_result : status_result {
    EndpointList endpoints;
};
template <class EndpointList>
resolve_result(status_result, EndpointList) -> resolve_result<EndpointList>;

struct write_result : status_result {
    // Bytes written
    size_t count;
    operator bool() const { return !error_code::operator bool(); }
};
struct partial_read_result : status_result, std::string_view {
    std::string_view message() { return *this; }
    operator bool() const {
        return status_result::good() && !status_result::eof();
    }
};
struct read_result : status_result, std::string {
    std::string message() { return *this; }
};
} // namespace conduit::async
