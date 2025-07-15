#include <thread>
#include <iostream>
void some_function()
{
    // for(int i=0;i<100;i++)
    std::cout << "some ";
}
void some_other_function()
{
    // for(int i=0;i<100;i++)
    std::cout << "other ";
}

int main()
{
    std::cout<<std::thread::hardware_concurrency;
    std::thread t1(some_function);
    std::thread t2 = std::move(t1);
    t1 = std::thread(some_other_function);
    std::thread t3;
    t3 = std::move(t2);
    if (t1.joinable())
        t1.join();
    t1 = std::move(t3);
    if (t1.joinable()) t1.join();
    std::cout<<std::thread::hardware_concurrency;
}
