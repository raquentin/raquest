module;

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

export module ThreadPool;

export class ThreadPool {
  public:
    explicit ThreadPool(size_t threads);

    /**
     * @brief Schedule a new task.
     *
     * @return A future to retrieve the result later.
     */
    template <class F, class... Args>
    auto submit(F &&f, Args &&...args)
        -> std::future<typename std::invoke_result_t<F, Args...>>;

    /**
     * @brief Signal all threads to stop and join them during destruction.
     */
    ~ThreadPool();

  private:
    std::vector<std::thread> workers_;

    /**
     * @brief A queue of function pointers, the work to be done.
     */
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cond_;
    bool stop_;
};

ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; i++) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    cond_.wait(lock,
                               [this] { return stop_ || !tasks_.empty(); });

                    if (stop_ && tasks_.empty())
                        return;

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

template <class F, class... Args>
auto ThreadPool::submit(F &&f, Args &&...args)
    -> std::future<typename std::invoke_result_t<F, Args...>> {
    using return_type = typename std::invoke_result_t<F, Args...>;

    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task_ptr->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        if (stop_)
            throw std::runtime_error("submitted on a stopped ThreadPool");

        tasks_.emplace([task_ptr] { (*task_ptr)(); });
    }

    cond_.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    cond_.notify_all();

    for (std::thread &worker : workers_)
        worker.join();
}
