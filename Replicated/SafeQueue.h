//
// Created by jdhyd on 2/8/2024.
//

#ifndef CSC8503_SAFEQUEUE_H
#define CSC8503_SAFEQUEUE_H

#include <thread>
#include <mutex>
#include <queue>


template <typename T>
class SafeQueue {
public:
    void Push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
    }

    [[nodiscard]] T Pop() {
        std::lock_guard<std::mutex> lock(mutex);
        auto value = queue.front();
        queue.pop();
        return value;
    }

    bool IsEmpty() {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex);
        while(!queue.empty())queue.pop();
    }

private:
    std::mutex mutex;
    std::queue<T> queue;
};


#endif //CSC8503_SAFEQUEUE_H
