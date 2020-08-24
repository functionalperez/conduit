#pragma once

#include <conduit/async/callback.hpp>
#include <conduit/mixin/resumable.hpp>

#include <conduit/io/responses.hpp>

#include <boost/asio.hpp>

#include <coroutine>

namespace conduit::io {
using boost::asio::ip::tcp;
using boost::system::error_code;
template <class...>
struct resolve;

template <class Protocol, class Executor>
class resolve<boost::asio::ip::basic_resolver<Protocol, Executor>>
  : mixin::Resumable<
        resolve<boost::asio::ip::basic_resolver<Protocol, Executor>>> {
    using super = mixin::Resumable<
        resolve<boost::asio::ip::basic_resolver<Protocol, Executor>>>;
    friend class mixin::Resumable<
        resolve<boost::asio::ip::basic_resolver<Protocol, Executor>>>;

    using resolver = boost::asio::ip::basic_resolver<Protocol, Executor>;
    using endpoint_list = typename resolver::results_type;
    resolver& r;
    std::string_view host;
    std::string_view service;
    error_code const* ec_ptr;
    endpoint_list endpoints;

    auto get_handler(std::coroutine_handle<> h) {
        return [this, caller = async::callback(h)](error_code const& ec,
                                            endpoint_list r) mutable {
            ec_ptr = &ec;
            endpoints = std::move(r);
            caller.resume();
        };
    }
    void on_suspend(std::coroutine_handle<> h) {
        r.async_resolve(host, service, get_handler(h));
    }

   public:
    resolve(resolver& r, std::string_view host, std::string_view service)
      : r(r), host(host), service(service) {}

    using super::await_ready;
    using super::await_suspend;
    auto await_resume() {
        return resolve_result{*ec_ptr, std::move(endpoints)};
    }
};

template <class Protocol, class Executor>
resolve(boost::asio::ip::basic_resolver<Protocol, Executor>&, std::string_view,
        std::string_view)
    -> resolve<boost::asio::ip::basic_resolver<Protocol, Executor>>;
} // namespace conduit::async
