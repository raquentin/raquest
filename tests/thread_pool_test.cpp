#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>
#include <vector>

import ThreadPool;

/**
 * @brief Trivially run a task.
 */
TEST(ThreadPoolTest, SingleTaskReturnValue) {
    ThreadPool pool(2);
    auto future = pool.submit([] { return 42; });
    EXPECT_EQ(future.get(), 42);
}

/**
 * @brief A few more tasks at once.
 */
TEST(ThreadPoolTest, MultipleTasksResult) {
    ThreadPool pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.submit([i] { return i * i; }));
    }

    for (int i = 0; i < 10; ++i) {
        int result = futures[i].get();
        EXPECT_EQ(result, i * i);
    }
}

/**
 * @brief Expect safe modification of shared atomic var.
 */
TEST(ThreadPoolTest, SharedAtomicIncrement) {
    ThreadPool pool(4);

    std::atomic<int> counter{0};

    // several tasks increment counter
    const int numTasks = 100;
    for (int i = 0; i < numTasks; ++i) {
        pool.submit(
            [&counter] { counter.fetch_add(1, std::memory_order_relaxed); });
    }

    // kinda chopped, but give some time for the tasks to queue up
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(std::memory_order_relaxed), numTasks);
}

/**
 * @brief Re-throw exceptions.
 */
TEST(ThreadPoolTest, ExceptionPropagation) {
    ThreadPool pool(2);

    auto future = pool.submit([] {
        throw std::runtime_error("Something bad happened!");
        return 123; // never reached
    });

    // should re-throw when we call get()
    EXPECT_THROW({ future.get(); }, std::runtime_error);
}

/**
 * @brief Just spam it.
 */
TEST(ThreadPoolTest, ManyTasksStressTest) {
    const int numThreads = 8;
    const int numTasks = 1000;

    ThreadPool pool(numThreads);
    std::atomic<int> sum{0};

    std::vector<std::future<void>> futures;
    futures.reserve(numTasks);
    for (int i = 0; i < numTasks; ++i) {
        futures.push_back(pool.submit([&sum] {
            // space heater
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            sum++;
        }));
    }

    for (auto &f : futures) {
        f.get();
    }

    EXPECT_EQ(sum.load(), numTasks);
}
