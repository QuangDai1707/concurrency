# Thread
- every thread has to have an *initial function*, where the new thread of execution begins.

# 2. Managing thread
## 2.1. Launching
```cpp
void do_some_work();
std::thread my_thread(do_some_work);
```
## 2.2. Transfer ownership
- use move semantic
- thread need to be not associated with any running thread

# 3. Sharing data
## 3.1. Data race
- a race condition is anything where the outcome depends on the relative ordering of execution of operations on two or more threads
- Most of the time, race is quite benign, such as enqueue which doesn't matter the insertion order
- when the race condition leads to broken invariants such as update linkedlist node when delete a node (1 updated and 1 not yet which lead to inconsistence in short period)

### 3.1.1. Avoid race
- 1. wrap your data structure with a **protection mechanism** to **ensure that only the thread performing a modification** 
- 2. design of your data structure and its invariants so that **modifications are done as a series of indivisible changes**, which preserves the invariants. (*lock-free programming*)
- 3. handle the updates to the data structure as a *transaction*

### 3.1.2. Protect with Mutex
- Before accessing a shared data structure, you *lock* the mutex associated with that data, and when you've finished accessing the data structure, you *unlock* the mutex.
- Use std::lock_guard class template, which implements that RAII idiom for a mutex.


### 3.1.4. Share Mutex
1. Exclusive lock
```c
std::lock_guard<std::shared_mutex>
std::unique_lock<std::shared_mutex>
```
    - only this thread can access critical section
    - it can only acquire an exclusive lock when the mutex is unlocked

2. Shared lock
```c
std::shared_lock<std::shared_mutex>
```
    - a thread with shared lock can enter critical section
    - it can only acquire shared lock if there is no exclusive lock

3. Data race avoidance
- The writer thread cannot get an exclusive lock if:
    - until all other threads release their lock
    - those threads have now left their critical sections
- The writer can acquire an exclusive lock when:
    - it enter critical section
    - reader threads cannot get a shared lock
    - writer threads cannot get an exclusive lock
    - until this thread release the lock

- The reader thread cannot get a shared lock
    - until the writer release its exclusive lock
    - the writer has now left the critical section
- The reader thread acquire the shared lock 
    - it enter critical section
    - other reader also can get a shared lock

3. Pros and Cons
- Use more memory and slower than std::mutex
- best suite for:
    - reader threads greatly outnumber writer threads
    - read operations take a long time

### Shared data
- static variables are thread-safe by default c11 standard (the first thread will intialize them and subsequence thread need to wait until the intialization success)
```c
#include <mutex>
class Test {
public:
	void func() { /*...*/ }
};
Test *ptest = nullptr;            // Variable to be lazily initialized
std::mutex mut;
void process() {
    if (!ptest) {                     // First check of ptest
        std::lock_guard<std::mutex> lck_guard(mut);
        // if (!ptest)                  // Second check of ptest
        ptest = new Test;   // Initialize ptest
    }
     ptest->func();
}
```
- why double-check ptest?
    - thread A check ptest which is null
    - thread B check ptest which is null
    - thread B lock the mutex 
    - thread B initialize ptest
    - thread B release mutex
    - thread A lock the mutex 
    - thread A initialize ptest (data race)

- a = new A; (involve 3 operations)
    - allocate enough memory to store this object
    - initialize memory, by constructing object to memory
    - store the memory address to pointer variable
- c++ allows these to be performed in difference order (memory order) such as 1-2-3 or 1-3-2     
- So this is not thread-safe:
    - thread A check the ptest and lock the mutex
    - thread A allocate memory and assign to ptest (1 to 3)
    - however, it has not yet called the constructor (2)
    - thread B check ptest and its not null
    - thread B does not lock the mutex
    - thread B jump out of the if statement
    - thread B call the member function of uninitialized object (undefined behavior)

- Solve this by std::call_once():
    - a given function is only called once
    - it is done in one thread
    - the thread cannot be interrupted until the function call completes

- the 4 way to do thread-safe lazy initialization
    - naive use of a mutex
    - use call_once (recommend)
    - double check locking wih c17 compiler or later (which has defined standard for initialize ordering: 1-2-3)
    - meyer singleton with static variable (most recommend)
    ```c
            void process()
        {
            static Test ptest;            // Variable which is lazily initialized
            ptest.func();
        }
    ```

## 4. Deadlock
- a thread is deadlocked when it cannot run
- often refer as mutual deadlock (A wait B, B wait A for an event that may never happen)
### 4.1. avoid deadlock
    - lock mutexes in order
    - use scope_lock to lock multiple mutexes at the same time (>=c17)
    - use std::lock() (similar to scope_lock)   (<c17)
        - unique_lock(mutex, std::defer_lock) -> std::lock()
        - std::lock() -> unique_lock(mutex, std::adopt_lock)
    - use std::try_lock()
    - use try_lock with timeout
    - use hierarchical ordering: 
        - lock lower-number mutex first
### 4.2. deadlock avoidence guildline
    - avoid waiting for thread while holding a lock
        - the other thread may need the lock to process
    - try to avoid waiting for other thread
        - the other threads may be waiting for your thread
    - try to avoid nested locks
        - if your thread already hold a lock, do not acquire another one
        - if you need multiple locks, acquire them in single operation
    - avoid calling functions with critical section
        - unless you ensure the function does not try to lock

## 4.3. livelock 
- a program cannot make progress
    - in deadlock, the thread are inactive
    - in livelock, the thread are still active
- livelock can result from badly done deadlock avoidance
    - a thread cannot get lock
    - instead of blocking indefinitely, it backs off and tries again
      
# 4. Thread Synchronization
## 4.1. Conditional variable
- Motivation: when thread need to coordinate with each other and avoid data race
- std::condition_variable
    - wait()
        - take an argument of type std::unique_lock
        - it **unlocks its lock** and **blocks the thread** until a notification is received
    - wait_for() and wait_until()
        - re-lock their argument if a notification is not received in time
    - notify_one()
        - wake up 1 of the waiting threads
        - the scheduler decides which thread is woken up
    - notify_all()
        - wake up all the waiting threads
- Scenario:
    - Thread A tell the conditional variable it is waiting
        - A lock the mutex then call the wait() of cv
        - cv unlock the mutex and block A thread
    - Thread B notifies the cv when it updates the string
        - B modify string and unlock the mutex
        - call notify_one()
    - the cv wakes thread A up 
        - wait() call return with mutex locked
    - thread A then uses the string
- Predicate: use to avoid
    - Lost wakeup: cv notify is called before wait() is called
        - the wait thread can process normally if notify is called before wait (thread just check the predicate)
    - Spurious wakeup: occasionally, system wakeup thread without notify 

# 4.2. Future and Promise
- std::thread does not provide a way to return a value
    - so far, only use the shared variables
    - cannot directly transfer data from 1 thread to another
- std::future and std::promise
    - class for transferring data between threads
    - together, these set up a "shared state" between threads
    - the shared state can transfer data from one thread to another
        - no shared data variable
        - no explicit locking
    - use producer-consumer model 
        - promise: producer thread generates result and stores it in the shared state
        - future: consumer thread waits and reads the result from the shared state
            - represents a result that is not yet available
            - call the future object's function but being blocked untils the result becomes available
            - not usuallly created directly: obtained from an std::promise or from return by an asynchrous operation
    - std::future
        - get():
            - obtains the result when ready
            - blocks until the operation is complete
            - feches the result and returns it
        - wait() 
            - block but do not return a result
            - wait_for and wait_until block with a timeout
    - std::promise:
        - constructor:
            - create an associate object
            - sets up the shared state with it
        - get_future()
            - return the associated future
        - set_value(): set the result to its argument
        - set_exeption: store thrown exeption in shared state
    - in case of multiple consumer, future cannot copied -> invalid -> use shared_future
        - future.share() and copy

# 5. Atomic types
## 5.1. why need atomic
- the increment (++) operation is a single instruction but involves 3 operation
    - pre-fetch the value of variable
    - increment the value in the processor core's register
    - publish the new value of count
- thread could use a outdate value in its calculation
- thread could publish its result afer a thread which ran later
- when declare a variable is atomic
    - the compiler will generate special instructions which
        - disable pre-fetch for variable
        - flush the store buffer immediately after doing the increment
    - avoid other problems such as hardware or compiler optimizations which change the instruction order
    - the result is that only one thread can access count at a time -> prevents data race

## 5.2. <atomic>
- all operations on the variable will be atomic
    - atomic<int> a;
- the parameter must be a type which is "trivially copyable"
    - scalar type
    - class where all the copy and move constructors are available
- able to assign to and from the object
    - x = 2; //atomic assignment to x
    - y = x; //atomic assignment from x. y can be non-atomic
- these are 2 distinct atomic operations that other threads could interleave between them
- std::atomic_flag
    - atomic boolean type but less overhead than atomic<bool>
    - only has 3 operation:
        - clear() set flag to false
        - test_and_set() sets flag to true and returns previous value
        - operator =()
    - must be initialized to false:
        - atomic_flag lock = ATOMIC_FLAG_INIT;
- spin lock
    - an infinite loop that keeps spinning until a condition becomes true
    - a alternatve to locking a mutex or using a cv
    - use std::atomic_flag to implement a basic spin lock
        - each thread calls test_and_set() in a loop
        - if this returns true
            - some other thread has set the flag and is in the critical seciton
            - iterate again
        - if it returns false
            - this thread has set the flag
            - exit the loop and proceed into the critical section
        - after the critical section, set the flag to false to allow another thread to execute in the critical section

# 6. Asynchrous 
## 6.1. packaged_task
- Turn a callable (function/lambda) into something that can run later, and whose result can be retrieved via a std::future.
- Replace the manually wire up between promise and future which reduce boilerplate code
    ```c
    std::packaged_task<int()> task([] { return 42; });
    std::future<int> fut = task.get_future();

    std::thread worker(std::move(task));  // automatically sets the result
    std::cout << fut.get();  // waits
    worker.join();
    ```
    - instead of
    ```c
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    std::thread producer([&] {
        prom.set_value(42);  // manually deliver the result
    });
    std::cout << fut.get();  // blocks until result is available
    producer.join();
    ```
## 6.2. std::async()
- higher level abstraction than std::thread
    - execute task with async which runs in the background
    - allow caller to do other work while the task is running
    - it can run synchrounously in the same thread
    - similar syntax to std::thread's constructor
- std::async()
    - return std::future object 
    - this can be in a different thread from the call to std::async()
    ```c
    auto result = std::async(fibonacci, 44);
	bool finished = false;
	using namespace std::literals;
	while (result.wait_for(1s) != std::future_status::ready) {
		std::cout << "Waiting for the result...\n";
	}
	// Call get() when we are ready
	std::cout << result.get() << std::endl;
    ```
- sdt::async() lauch flags
    - std::launch::async
        - a new thread is started for the task
        - the task is executed as soon as the thread starts
    - std::launch::deferred
        - nothing happens until get() is called on the returned future
        - the task is then executed ("lazy evaluation")
    - if both flags are set
        - the implementation decides whether to start a new thread 
        - this is the default
    - default launch policy
        - lack of certainty
            - the task could execute synchronously with the initiating thread 
            - the task could execute concurrently with the initiating thread   
            - it could execute concurrently with the thread that calls get()
            - if get() is not called, the task may not execute at all
        - thread-local storage (TLS)
            - we do not know which thread's data will be used
    - use the async launch option if any of these are true
        - the task must execute in a separate thread
        - the task must start immediately
        - the task will use thread-local storage
        - the task function must be executed, even if get() is not called
        - the thread receiving the future will call wait_for() or wait_until()
    - use the deferred launch option if 
         - the task must be run in the thread which calls get()
         - the task must be executed, even if no more threads can be created
- return value from wait() and friends
    - wait: return nothing
    - wait_for and wait_until
        - std::future_status::ready if the result is available
        - std::future_status::timeout if the timeout has expired
        - std::future_status::deferred if the result is being lazy evaluated

## 6.3. Choosing a thread object
- std::async
    - advantages: the simplest way to execute a task
        - easy to obtain the return value or catch exception from a task
        - choice of running task sync or async through the launch flags
        - the lib manage threads and inter-thread communication that no need to use shared data
    - disadvantages: 
        - cannot detach tasks
        - a task executed with std::lanch::async is "implicitly joined"
            ```c
            {
                auto fut = std::async(std::lanch::async, hello);
            } // Calls ~fut()
            ``` 
        - the returned future's desconstructor will block until the task completes or if get() or wait() doesnt call
            ```c
            void task()
            {
                std::this_thread::sleep_for(5s);
                std::cout << "Task result: " << 42 << '\n';
            }

            void func()
            {
                std::cout << "Calling async\n";
                auto result = std::async(std::launch::async, task); // block the execution until the task is completed
            }

            int main()
            {
                func();
                std::cout << "Task started\n";
            }
            ```
- std::packaged_task
    - advantages:
        - the best choice if we want to represent tasks as objects
        - lower level abstraction than std::async()
            - can control when a task is executed or on which trhead it is execute
- std::thread
    - advantages:
        - the most flexible
            - allows access to the underlying software thread
            - can be detached
            - useful for features that not supported by standard C++

# 7. Parallelism
- Concurrency
    - Describes conceptually distinct tasks
        - separation of concerns
        - can run on a single core
    - these tasks often interact
        - wait for an event
        - wait for each other
    - the tasks often overlap in time
    - concurrency is a feature of the program structure
- Parallelism
    - the tasks are identical
    - they all run at the same time on multiple cores to improve scalability
    - these tasks run independently of each other
    - parallelism is a feature of the algorithm being run
- Execution Policy

| Policy                      | What It Is                                                                  | When to Use It                                                                    | ✅ Pros                                                                                             | ⚠️ Cons                                                                                                                                  |
| --------------------------- | --------------------------------------------------------------------------- | ----------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `std::execution::seq`       | Sequential execution on a **single thread**, with **ordered** operations.   | Default behavior. Safe, predictable. Best for debugging or small tasks.       | ✅ Deterministic order<br>✅ Safe with any kind of code<br>✅ Simple to reason about                  | ❌ Slowest<br>❌ No use of hardware parallelism or vectorization                                                                           |
| `std::execution::unseq`     | **Unsequenced execution** with **SIMD** (vectorization) on a single thread.  | For **data-parallel** operations (e.g. math, image filters). *If code can modify shared state*            | ✅ Takes advantage of SIMD instructions<br>✅ Faster than `seq` for numeric loops                    | ❌ Undefined order of execution<br>❌ Still single-threaded<br>❌ Code must not have inter-iteration dependency                             |
| `std::execution::par`       | **Parallel execution** using **multiple threads**, but not vectorized.      | For CPU-bound tasks with large data sets. *Data race cannot occur, but code doesn't modify shared state*                                    | ✅ Utilizes multi-core CPUs<br>✅ Faster than `seq` and `unseq` for heavy work                       | ❌ Order not guaranteed<br>❌ Requires thread-safe code<br>❌ Overhead of thread management                                                 |
| `std::execution::par_unseq` | Full **parallel + vectorized execution**; uses threads and SIMD together.   | When you need **maximum performance** and your loop is **fully independent**. *Requirement is data race cannot occur or code doesn't modify shared state*  | ✅ Highest performance<br>✅ Fully hardware-accelerated<br>✅ Efficient for large compute-heavy tasks | ❌ Hardest to reason about<br>❌ Undefined execution order<br>❌ Must be free of all data dependencies<br>❌ Unsafe if not carefully written |

```c
	// Performed as 2 vectorized additions (0 + 1 + 2 + 3) + (4 + 5 + 6 + 7)
	auto sum = std::reduce(se::unseq, vec.begin(), vec.end(), 0);
    // Performed as 2 parallel vectorized additions ((0 + 1 + 2 + 3) + (4 + 5 + 6 + 7))
	auto sum = std::reduce(se::par_unseq, vec.begin(), vec.end(), 0);
```

- When to use Execution Policies
    - Dont:
        - Your code has to be portable to other compilers
        - The task is essentially sequential
        - operation order is important
        - the algorithm call throws exceptions
        - unless immediate termination is acceptable
        - prevent data race costs more than not using an execution policy
    - Do:
        - measurement shows a worthwhile improvement in performance
        - it can be done safely and correctly


# 8. Concurrency data structures
- objects contains many elements, modidy operations on an element can affect other parts of the object
- STL containers are "memory objects"
- Concurrenct reads of same object are safe, but concurrents reads and writes are not
- Coarse-grained locking
    - lock the entire object
        - easy because require no change in data structure  
    - sometimes the only option
        - build-in type, C++ std library types
        - type provided by other programmers, if we cannot modify them
    - in effect, all code that access the object will be single-thread 
- Fine-grained locking
    - we can choose which parts of the object to lock
    - allow concurrent access
    - require writing extra code
    - require careful design
    - increasing code of creating objects (mutex initialization)

## 8.1. Shared pointer
- Different instances can share the same memory location
- Include **memory pointer** and **control block** which contain **reference counter**
- when a shared_ptr object is copied or assigned
    - there is no memory operation
    - instead, the reference counter is incremented
    - when a copy is destroyed, the counter is decremented
    - when the last copy is destroyed, counter = 0, which allocated memory is then released 
- Create a shared_ptr object (in <memory>)
    ```c
    // pass a pointer as the constructor argument
    std::shared_ptr<int> ptr1(new int(42));
    // calling std::make_share() 
    auto ptr2 = std::make_share<int>(42);
    ```
- Copy a shared_ptr
    ```c
    // ptr2 and ptr3 share the same memory pointer and control block which has reference counter increase by 1
    std::shared_ptr<int> ptr3 = ptr2;
    ```
- Thread and shared_ptr
    - Reference counter is atomic type
        - Internal synchronization
        - make it safe in threaded programs but add extra overhead in single thread programs 
    - the point-to data is responsibility of the programmer
        - External synchronization
        - must be protected against data races which concurrent access to the data must be synchronized

## 8.2. Monitor class
- internally synchronized class
- naive solution
    - mutex is the private member
    - every member function which access shared data locks the mutex
    - drawbacks
        - member functions may need to call other member functions
            - use multiple locks risks deadlock
        - transactions may involve multiple member function calls
        - result in many locking and unlocking operation
            - race condition
            - potential data race
        - existing classes need to be modified
- monitor clas
    - write the wrapper class
        - the actual object is a data member
        - member functions lock a mutex and forward to the actual object
    - Advantages
        - Works with any type
            - include class that not designed for threaded code
            - no modification need to the class
        - does not help caller who want to perform transactions
            - unnecessary locking
            - possibility of deadlock due to multiple locking
            - allows interruptions by other threads
- sophisticated monitor class
    - make it generic with template parameter
    - functor class with overloaded operator()
        - the argument is a callable object 
        - this contains the sequence of member function calls for the transaction
        - we lock the mutex, then invoke the callable object
    - this will be a template member function
        - the type parameter is the type of the callable object
    ```c
    template <typename T>
    class Monitor {
        T data;
        std::mutex mut;
    public:
        // Default constructor
        Monitor<T>(T data = T{}) : data(data) {}
        // Function call operator
        // Argument is a callable object of type Func
        // which takes an argument of type T
        template <typename Func>
        auto operator() (Func func) {
            std::lock_guard<std::mutex> lck_guard(mut);
            
            // Call the function, protected by the lock
            return func(data);
        }	
    };

    int main()
    {
        // Transfer $1000 from Peter to Paul
        // Must be done as a single transaction	
        
        // Monitor wrapper for Bank class
        Monitor<Bank> mon;
        
        // Invoke its function call operator and
        // pass a callable object which takes a Bank argument
        mon([] (Bank bank) {
            // Call its member functions, all under the same lock
            bank.debit("Peter", 1000);
            bank.credit("Paul", 1000);
            bank.print("Peter");
            bank.print("Paul");
        });
        
        std::cout << "Done\n";
    }
    ```

## 8.3. Semaphore
- has a counter
- acquire()
    - decrements the counter
- release()
    - increments the counter
- the counter can be zero
    - acquire() will block until the counter becomes positive again
    ```c
    class Semaphore {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int counter{0};
    public:
        void release() {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Adding one item" << std::endl;

            ++counter;
            count();	

            cv.notify_all();
        }

        void acquire() {
            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "Removing one item" << std::endl;

            while (counter == 0) {
                cv.wait(lock);
            }

            --counter;
            count();
        }

        void count() const {
            std::cout << "Value of counter: ";
            std::cout << counter << std::endl;
        }
    };
    ```
- binary semphore as mutex
    - the counter can only have 2 values 0 and 1
    - used for mutex
    - to "lock" it, call acquire()
    - to "unlock" it, call release() in the same thread so other threads can acquire the lock
- binary as CV
    - also used for signalling: can be used as a replacement for cv
    - to wait for a signal, a threads acquire()
        - counter -> 0, thread waits for another thread to increment it
    - to notify a waiting thread, call release()
        - count -> 1, the waiting thread can now continue
    - to notify multiple threads, use a suitable value for max_count
- Advantages
    - more flexible: can notify any given number of waiting threads
    - simpler code: avoids working with mutexes and cv
    - performance: can often be faster
    - more versatile: can be used to create more complex synchronization objects     

## 8.3. concurrent queue
- add 2 condition variables for efficiently wait the queue in case of push when queue full or pop when queue empty
    ```c
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mut);
        not_full.wait(lock, [this] { return que.size() < max_size; });

        que.push(value);
        not_empty.notify_one();
    }
    void pop(T& value) {
        std::unique_lock<std::mutex> lock(mut);
        not_empty.wait(lock, [this] { return !que.empty(); });

        value = std::move(que.front());
        que.pop();
        not_full.notify_one();
    }
    ```
## 8.4. Thread pool
- motivations: create thread requires a lot of work
    - create an execution stack for the thread
    - call a system api
    - the os creates internal data to manage the thread
    - the scheduler executes the thread
    - a context switch occurs to run the thread
- we want to make full use of all our processor cores
- every core should be running one of our threads except for main() and the os
- difficult to achieve with std::async()
    - need to keep track of the number of threads created
- thread pool structure
    - container of C++ thread objects
        - has a fixed size
        - usually match to the number of core on machine (std::thread::hardware_concurrency()) subtract 2 (main thread + OS)
    - a queue of tasks
        - a thread takes a task off the queue
        - it performs the task
        - then it takes the next task from the queue
    - tasks represented as callable objects
- advantages
    - no scaling concerns
        - the thread pool will automatically use all the available cores
    - make efficient use of resource
        - threads are always busy
        - provided there is work for them to do
    - works best with short, simple tasks where
        - the time taken to create a thread causes a significant delay
        - the task doesn't block 
- disadvantages
    - requires a concurrent queue or similar
    - overhead: must add and remove task functions in thread-safe way
- thread pool implementation
    - thread pool class will contain
        - a vector of std::thread objects
        - a concurrent queue to store incoming tasks as callable objects
    - each thread will execute as an infinite loop
        - call pop() on the queue
        - invoke the task return by pop()
        - call pop() on the queue again
        - ...
- thread pool with multiple queues
    - the queue can become a bottle-neck
        - when a thread takes a task off the queue, it locks the queue
        - other threads are blocked until this operation is complete
        - if there are many small tasks, this can affect performance
    - an alternative is to use a separate queue for each thread
        - a thread never has to wait to get its next task
        - use more memory
    - if the queue is empty, the thread is idle
    - replace the queue by a fixed-size vector of queues (= number of threads)
    - "round-robin" scheduling
        - put a new task on the next thead's queue
        - after the last element of the vecotr, go to the front element