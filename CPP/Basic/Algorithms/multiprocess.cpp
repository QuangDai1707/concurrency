// multiprocess_example.cpp
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int counter = 0;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        for (int i = 0; i < 100000; ++i)
            ++counter;
        std::cout << "Child counter: " << counter << std::endl;
    } else {
        // Parent process
        for (int i = 0; i < 100000; ++i)
            ++counter;
        wait(nullptr);
        std::cout << "Parent counter: " << counter << std::endl;
    }

    return 0;
}
