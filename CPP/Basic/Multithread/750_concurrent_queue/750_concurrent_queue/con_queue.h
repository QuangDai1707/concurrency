#ifndef CONCURRENT_QUEUE_CV_H
#define CONCURRENT_QUEUE_CV_H

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

template <class T>
class concurrent_queue_cv {
private:
    std::mutex mut;
    std::queue<T> que;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    std::size_t max_size;

public:
    // Constructors
    explicit concurrent_queue_cv(std::size_t max = 50) : max_size(max) {}

    // Deleted copy/move semantics
    concurrent_queue_cv(const concurrent_queue_cv&) = delete;
    concurrent_queue_cv& operator=(const concurrent_queue_cv&) = delete;
    concurrent_queue_cv(concurrent_queue_cv&&) = delete;
    concurrent_queue_cv& operator=(concurrent_queue_cv&&) = delete;

    // Push: waits if queue is full
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mut);
        not_full.wait(lock, [this] { return que.size() < max_size; });

        que.push(value);
        not_empty.notify_one();
    }

    // Push with move semantics
    void push(T&& value) {
        std::unique_lock<std::mutex> lock(mut);
        not_full.wait(lock, [this] { return que.size() < max_size; });

        que.push(std::move(value));
        not_empty.notify_one();
    }

    // Emplace: constructs element in place
    template <typename... Args>
    void emplace(Args&&... args) {
        std::unique_lock<std::mutex> lock(mut);
        not_full.wait(lock, [this] { return que.size() < max_size; });

        que.emplace(std::forward<Args>(args)...);
        not_empty.notify_one();
    }

    // Pop: waits if queue is empty
    void pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        not_empty.wait(lock, [this] { return !que.empty(); });

        value = std::move(que.front());
        que.pop();
        not_full.notify_one();
    }

    // Try-pop: non-blocking pop
    bool try_pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        if (que.empty()) return false;

        value = std::move(que.front());
        que.pop();
        not_full.notify_one();
        return true;
    }

    // Check if empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mut);
        return que.empty();
    }

    // Get current size
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mut);
        return que.size();
    }
};

#endif // CONCURRENT_QUEUE_CV_H
