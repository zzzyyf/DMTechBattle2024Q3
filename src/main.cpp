#include "Producer.h"

#include <chrono>
#include <cstdio>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace dm;

const u32 gen_concurrency = 16;
const u32 nn = 10'000'000;

const u32 buffer_size_limit = 8 * 1024 * 1024;

String genString();

String output_path = "output.txt";


int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "this program only accepts output_path (optional) as only one argument" << std::endl;
        return 1;
    }

    if (argc == 2) {
        output_path = argv[1];
    }
    else {
        std::cout << "using default output_path " << output_path << ", you can pass custom path as argument" << std::endl;
    }

    // 1. gen string
    String init_str = genString();

    // 2. record t1
    auto t1 = std::chrono::steady_clock::now();

    // 3. gen and write all strings
    FILE *file = std::fopen(output_path.c_str(), "w+");
    if (!file)
    {
        std::cerr << "error opening output file" << std::endl;
        return 1;
    }

    Queue queue(gen_concurrency);

    Writer writer(file, queue);

    u32 n_task_per = nn / gen_concurrency;
    u32 remaining = nn % gen_concurrency;

    std::vector<std::thread> threads;
    for (u32 i = 0; i < gen_concurrency; i++)
    {
        u32 n = n_task_per;
        if (i < remaining) {
            n++;
        }

        threads.emplace_back([&, n]()
        {
            Producer producer(n, init_str, buffer_size_limit / gen_concurrency, queue);
            producer.produce();
        });
    }

    if (!writer.write())
    {
        std::cerr << "error writing output file" << std::endl;
        for (u32 i = 0; i < gen_concurrency; i++)
        {
            threads[i].join();
        }
        return 1;
    }

    // 4. close file
    std::fclose(file);

    // 5. record t2
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "cost " << (t2-t1).count() / 1000'000 << "ms." << std::endl;

    for (u32 i = 0; i < gen_concurrency; i++)
    {
        threads[i].join();
    }

    return 0;
}


std::string genString()
{
    std::random_device r;
    std::default_random_engine e1(r());

    std::uniform_int_distribution<int> char_uniform_dist(0, 25);

    u32 len = 600;
    std::string ss(len, 0);

    for (u32 i = 0; i < len; i++) {
        ss[i] = 'a' + char_uniform_dist(e1);
    }

    return ss;
}
