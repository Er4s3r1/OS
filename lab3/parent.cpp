#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <fstream>
#include <condition_variable>
#include <sstream>
#include <ctime>
#include <pthread.h>
#include <mutex>

int create_process(){
    pid_t pid = fork();
    if (-1 == pid){
        perror("fork");
        exit(-1);
    }
    return pid;
}

int sizeOfFile(std::string name){
    int k = 0;
    std::fstream file(name, std::ios::in);
    std::string tmp;
    while(std::getline(file, tmp)){
        k ++;
    }
    return k;
}


int main(){
    std::cout << "Enter file name" << std::endl;
    std::string filename;
    std::cin >> filename;

    int size = 4096;
    const char* name = "shared_memory";

    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0777);
    if (shm_fd == -1){
        std::cout << "Error with fd\n";
        return 1;
    }
    if (ftruncate(shm_fd, size) == -1){
        std::cout << "error with trunc" << std::endl;
        return 1;
    }

    void* shm_ptr = mmap(nullptr , size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm_ptr == MAP_FAILED) {
        std::cout << "Error mapping shared memory" << std::endl;
        munmap(shm_ptr, size);
        close(shm_fd);
        return 1;
    }
   
    int* arr = (int*)(shm_ptr);
    arr[0] = 1;

    pid_t pid = create_process();
    if (0 == pid){
        // child process
        
        
        int FD = open(filename.c_str(), O_RDONLY);
        if (FD == -1){
            munmap(shm_ptr, size);
            close(shm_fd);
            close(FD);
            std::cout << "Error opening file" << std::endl;
            return 1;
        }
       
        if (dup2(FD, STDIN_FILENO) == -1){
            munmap(shm_ptr, size);
            close(shm_fd);
            close(FD);
            std::cout << "Error dup2" << std::endl;
            return 1;
        } // in -> file
        close(FD);

        if (execl("./child", "child", name, NULL) == -1){
            perror("exec error");
            return -1;
        }
        
    }
    else{
        
        std::cout << "Data from shared memory:" << std::endl;
        while(1){
            if (arr[0] == 2){
                break;
            }
            if (arr[0] == 0){
                if (arr[1] == -1e9){
                    std::cout << "Division by 0\n";
                }
                else{
                    std::cout << arr[1] << "\n";
                }
                
                arr[0] = 1;
            }
        }

        munmap(shm_ptr, size);
        close(shm_fd);
        
    }
    return 0;
}
