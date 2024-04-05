#include "ThreadPool.h"

int main()
{
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(pool.pushTasks([i] {return i * i;}));
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;
}
