#ifndef THREAD_SAFE_QUEUE_INCLUDED_HPP
#define THREAD_SAFE_QUEUE_INCLUDED_HPP

#include <mutex>
#include <queue>
#include <condition_variable>

#if __cplusplus >= 201703L
#include <optional>
#endif

template <typename T>
class ts_queue {
public:
    ts_queue() = default;

    ts_queue(ts_queue &&rhs) {
        std::lock_guard<std::mutex> lock(mutex);
        data = std::move(rhs.data);
    }

    ~ts_queue() {}

    ts_queue(const ts_queue& rhs) = delete;

    ts_queue &operator=(const ts_queue& rhs) = delete;

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data.empty();
    }

    void push(const T& item)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(item);
        cv.notify_one();
    }

    void push(T&& item)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(std::move(item));
        cv.notify_one();
    }

    T wait_and_pop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        cv.wait(lock, [this] () { return !data.empty(); });
        T current = data.front();
        data.pop();
        return current;
    }

#if __cplusplus >= 201703L
    std::optional<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty()) return {};
        T current = data.front();
        data.pop();
        return current;
    }
#else
    bool try_pop(T& item)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty()) return false;
        item = data.front();
        data.pop();
        return true;
    }
#endif

private:
    std::queue<T>            data;
    mutable std::mutex       mutex;
    std::condition_variable  cv;
};

#endif //THREAD_SAFE_QUEUE_INCLUDED_HPP
