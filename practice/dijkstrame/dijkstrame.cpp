#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>   
#include <sys/types.h>
#include <sys/wait.h> 
#include <vector>
#include <iostream>
#define NUM_OF_PROCESS 3

int main(){
    pid_t pid;
    std::vector<pid_t> pids;
    for (int i=0; i<NUM_OF_PROCESS; i++) {
        pid = fork();
        if(pid != 0){
            pids.push_back(pid);
            break;
        }
        else{
            continue;
        }
    }
    if(pid == 0){
        std::cout << "Parent Node is alive" << std::endl;
    }
    if(pid > 0){
        /* Critical Region Starts*/
        std::cout << "This is child pid:" << pid << std::endl;
        exit(EXIT_SUCCESS);
        /* Critical Region Ends*/
    }
    else{
        for(int i=0; i<NUM_OF_PROCESS; i++){
            waitpid(pids[i], NULL, 0);
        }
    }
    std::cout << "Parent process will exit now." << std::endl;
    return 0;
}


