#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <fcntl.h>
#include <sstream>
#include <sys/mman.h>

int main(int argc, char* argv[]){
    int size = 4096;
    int shm_fd = shm_open(argv[1], O_CREAT | O_RDWR, 0777);
    void* shm_ptr = mmap(nullptr , size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
   
    
    int* arr = (int*)shm_ptr;

    int res=1;
    int number;
    int i = 0;
    int newCommand = 1;


    while(std::cin.peek() != EOF){
        if (newCommand){
            std::cin>>res;
            newCommand = 0;
        }
        if (std::cin.peek() == '\n'){
            std::cin.ignore();
            while(1){
                if (arr[0] == 1){
                    arr[1] = res;
                    arr[0] = 0;
                    break;
                }
            }
            newCommand = 1;

        }
        
        if (!newCommand){
            std::cin >> number;
            if (number == 0){
                while(1){
                    if (arr[0] == 1){
                        arr[1] = -1e9;
                        arr[0] = 0;
                        break;
                    }
                }   
                break;
            }
            else{
                res /=number;
            }
           
        }
    }
    while(1){
        if (arr[0] == 1){
            arr[0] = 2;
            break;
        }
    }
    

}
