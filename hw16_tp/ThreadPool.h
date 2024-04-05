#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>

class ThreadPool {
private:
    std::vector<std::thread> working_threads;
    std::queue<std::function<void()>> tasks_queue;
    std::mutex queue_mutex;
    std::condition_variable condition; // Condition variable for task notification
    bool stop; // Flag to stop the thread pool

public:
// Constructor starts a number of worker threads
    explicit ThreadPool(size_t number_of_threads) : stop(false){
        for(size_t i = 0; i < number_of_threads; ++i)
            working_threads.emplace_back(
                [this]{
                    for(;;){
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, [this]{ return this->stop || !(this->tasks_queue.empty()); });
                            if(this->stop && this->tasks_queue.empty())
                                return;
                            task = std::move(this->tasks_queue.front());
                            this->tasks_queue.pop();
                        }
                        task();
                    }
                }
            );
    }
    // Add new work item to the pool
    template<class type_of_callable, class... Args>  // F-y kara lini kanchvox lyuboy tipi
    auto pushTasks(type_of_callable&& instance_of_callable, Args&&... args)  //(instance_of_callable) - threadnerov katarvox foo
    -> std::future<typename std::result_of<type_of_callable(Args...)>::type>
    {
        using return_type = typename std::result_of<type_of_callable(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>( // ptr<foo<veradarznox tip>>
                std::bind(std::forward<type_of_callable>(instance_of_callable), std::forward<Args>(args)...)
        ); // forward - ssillkayi aveli lav tarberak a

        std::future<return_type> result = task->get_future(); // return type zavernut v future i yavlyaetsa tipom result
        { // get future-ov stanum em taski rezultaty
            std::unique_lock<std::mutex> lock(queue_mutex);

            if(stop)
                throw std::runtime_error("push on stopped ThreadPool");

            tasks_queue.emplace([task](){ (*task)(); });
        }
        condition.notify_one(); // tasky anelu hamar mi hat thread a kanchum
        return result; // uzum a stana taski rezultaty
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: working_threads)
            worker.join(); // threadery irar a kpcnum
    }
};