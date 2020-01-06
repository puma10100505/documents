#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <vector>

#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/bind/bind.hpp"

void hello()
{
    std::cout << "hello a single thread" << std::endl;
}

boost::mutex io_mutex;

std::vector<int> data;
boost::chrono::milliseconds interval(50);

std::vector<int> result;

void recv()
{
    while (true)
    {
        if (data.size() <= 0)
        {
            boost::this_thread::sleep_for(interval);
        }
        else
        {
            for (const int &item : data)
            {
                result.push_back(item);
            }

            data.clear();

            if (result.size() >= 10)
            {
                break;
            }
        }
    }
}

void send()
{
    for (int i = 0; i < 10; i++)
    {
        data.push_back(i);
        if (data.size() > 0)
        {
            boost::this_thread::sleep_for(interval);
        }
    }
}

int main()
{
    boost::thread th1(&send);
    boost::thread th2(&recv);
    th1.detach();
    th2.detach();

    std::cout << result.size() << std::endl;
    for (const int &num : result)
    {
        std::cout << num << std::endl;
    }

    return 0;
}
