#pragma once
#include <conduit/coroutine_queue.hpp>

namespace conduit {

class scheduler {
    coroutine_queue jobs;
    std::atomic_bool running = true;

   public:
    struct await_post {
        scheduler& c;
        coroutine_node node;
        constexpr bool await_ready() const {
            return false;
        }
        void await_suspend(std::coroutine_handle<> h) noexcept {
            node.caller = h;
            c.post(node);
        }
        constexpr void await_resume() {}
    };
    scheduler() = default;
    void post(coroutine_node& node) noexcept { jobs.enqueue(node); }
    await_post post() {
        return await_post{*this};
    }

    void stop() { running = false; }
    void run() {
        while (running) {
            coroutine_node* job = static_cast<coroutine_node*>(jobs.dequeue());
            if (job == nullptr)
                break;
            job->caller();
        }
    }
    void operator()() {
        run();
    }
};
} // namespace conduit
