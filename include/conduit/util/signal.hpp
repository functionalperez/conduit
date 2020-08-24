#include <csignal>
#include <functional>

namespace conduit {
std::function<void(int)>& get_sigint() {
    static std::function<void(int)> f = [](int) {};
    return f;
}
void run_sigint(int s) { get_sigint()(s); }
void set_sigint(std::function<void(int)> f) {
    get_sigint() = std::move(f);
    std::signal(SIGINT, run_sigint);
}
} // namespace conduit