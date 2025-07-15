#include <thread>
#include <iostream>

int main()
{
    std::thread t1([]{
        for(int i=0;i<1000;i++){
            std::cout<<1<<" ";
        }
    });
    std::thread t2([]{
        for(int i=0;i<1000;i++){
            std::cout<<2<<" ";
        }
    });
    std::thread t3([]{
        for(int i=0;i<1000;i++){
            std::cout<<3<<" ";
        }
    });

    // Now wait
    t1.join();
    t2.join();
    t3.join();
}