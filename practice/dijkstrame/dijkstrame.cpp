#include <iostream>
#include <vector>
#include <atomic>
#include <cstdlib> 
#include <ctime>  
#include <unistd.h> 
#include <sys/mman.h> 
#include <sys/wait.h>
#include <sched.h>  
#include <stdexcept> 
#include <chrono>   
#include <thread>

// Shared Memory for flags by allocating memory to pointers
// [ std::atomic<int> turn ]
// [ std::atomic<int> flag[0] ]
// [ std::atomic<int> flag[1] ]
// ...
// [ std::atomic<int> flag[N-1] ]

// Global pointer for shared memory
void* shared_mem_global = nullptr;
size_t shared_mem_size_global = 0;

void critical_section(int process_id) {
    std::cout << "Process " << process_id << " (PID: " << getpid() << ") entering Critical Section." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Process " << process_id << " (PID: " << getpid() << ") exiting Critical Section." << std::endl;
}


void remainder_section(int process_id) {
    std::cout << "Process " << process_id << " (PID: " << getpid() << ") entering Remainder Section." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100 + 50));
    std::cout << "Process " << process_id << " (PID: " << getpid() << ") exititng Remainder Section." << std::endl;

}

// Dijkstra's Algorithm for Process
void process_function(int i, int N, std::atomic<int>* turn, std::atomic<int>* flags) {
    srand(time(nullptr) + i); // Seed random number generator differently for each process

    while (true) {
        remainder_section(i);

    //Subroutine L
    L:
        flags[i].store(1);
        while (turn->load() != i) {
            int current_turn = turn->load();
            if (current_turn >= 0 && current_turn < N && flags[current_turn].load() == 0) {
                turn->compare_exchange_strong(current_turn, i);
            }
             sched_yield();
        }

        flags[i].store(2); 

        bool contention = false;
        for (int j = 0; j < N; ++j) {
            if (i == j) continue; 
            if (flags[j].load() == 2) {
                contention = true;
                break; 
            }
        }

        if (contention) {
            sched_yield(); 
            goto L; 
        }

        critical_section(i);
        flags[i].store(0); // Reset flag
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <num_processes>" << std::endl;
        return 1;
    }
    int N = std::atoi(argv[1]);
    if (N <= 0) {
         std::cerr << "Number of processes must be positive." << std::endl;
         return 1;
    }

    // Allocate Shared Memory 
    size_t turn_size = sizeof(std::atomic<int>);
    size_t flags_size = N * sizeof(std::atomic<int>);
    size_t total_size = turn_size + flags_size;
    shared_mem_size_global = total_size;

 
    void* shared_mem = mmap(NULL,                      
                           total_size,             
                           PROT_READ | PROT_WRITE,    
                           MAP_SHARED | MAP_ANONYMOUS,
                           -1,                        
                           0);                        

    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }
    shared_mem_global = shared_mem; 

    std::atomic<int>* turn_ptr = static_cast<std::atomic<int>*>(shared_mem);
    std::atomic<int>* flags_ptr = reinterpret_cast<std::atomic<int>*>(static_cast<char*>(shared_mem) + turn_size);

    turn_ptr =  new std::atomic<int>(0); 
    for (int i = 0; i < N; ++i) {
        new (flags_ptr + i) std::atomic<int>(0);
    }

    // Fork Processes
    std::vector<pid_t> child_pids;
    pid_t pid;

    std::cout << "Parent process (PID: " << getpid() << ") creating " << N << " child processes..." << std::endl;

    for (int i = 0; i < N; ++i) {
        pid = fork();

        if (pid == 0) { 
            process_function(i, N, turn_ptr, flags_ptr);
            exit(0); 
        } else { 
            child_pids.push_back(pid); 
            std::cout << "Parent created child " << i << " (PID: " << pid << ")" << std::endl;
        }
    }

    std::cout << "Parent process (PID: " << getpid() << ") waiting for children..." << std::endl;
    int status;
    for (pid_t child_pid : child_pids) {
        if (waitpid(child_pid, &status, 0) < 0) {
            perror("waitpid failed");
        } else {
             std::cout << "Parent observed child PID " << child_pid << " finished with status " << status << "." << std::endl;
        }
    }

     std::cout << "Parent process cleaning up shared memory..." << std::endl;

    turn_ptr->~atomic();
    for (int i = 0; i < N; ++i) {
        (flags_ptr + i)->~atomic();
    }

     shared_mem_global = nullptr; // Clear global pointer

    std::cout << "Parent process finished." << std::endl;
    return 0;
}