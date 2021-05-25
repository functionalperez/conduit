#pragma once
#include <atomic>
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit {
struct coroutine_node {
    std::atomic<coroutine_node*> next = nullptr;
    std::coroutine_handle<> caller;
};
class coroutine_queue {
    using next_pointer = std::atomic<coroutine_node*>*;
    std::atomic<coroutine_node*> head;
    std::atomic<next_pointer> last_ptr;

   public:
    coroutine_queue() noexcept : head(nullptr), last_ptr(&head) {}
    // Atomically enqueues a node
    inline void enqueue(coroutine_node& n) noexcept {
        next_pointer previous = last_ptr.exchange(&n.next);
        *previous = &n;
    }
    // Atomically removes a node from the queue.
    // Returns null if the queue is empty.
    inline coroutine_node* dequeue() noexcept {
        coroutine_node* old_head = head;
        coroutine_node* new_head;
        do {
            if (old_head == nullptr)
                return nullptr;
            next_pointer old_head_next = &(old_head->next);
            last_ptr.compare_exchange_strong(old_head_next, &head);
            new_head = old_head->next;
        } while (!head.compare_exchange_weak(old_head, new_head));

        old_head->next = nullptr; // Reset the next node to null
        return old_head;
    } 
    bool empty() noexcept { return head == nullptr; }
    ~coroutine_queue() {
        while (coroutine_node* node = dequeue()) {
            node->caller.destroy();
        }
    }
};
} // namespace conduit