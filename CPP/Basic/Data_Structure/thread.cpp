// C++ Program to illustrate the usage of <atomic> Header
#include <atomic>
#include <iostream>
#include <thread>
using namespace std;

atomic<int> counter(0); // Atomic integer
int c = 0;
void increment_counter(int id)
{
    for (int i = 0; i < 1000; ++i)
    {
        // Increment counter atomically
        cout << counter.load() << " ";
        counter.fetch_add(id);
    }
}

void increment(int id)
{
    for (int i = 0; i < 1000; ++i)
    {
        // Increment counter atomically
        cout << c << " ";
        c += id;
    }
}

int main()
{
    thread t1(increment_counter, 1);
    thread t2(increment_counter, 2);

    t2.join();
    t1.join();

    cout << endl
         << "Counter: " << counter.load() << endl;

    thread t3(increment, 1);
    thread t4(increment, 2);

    t4.join();
    t3.join();

    cout << endl
         << "Counter: " << c << endl;

    return 0;
}