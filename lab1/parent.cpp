#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <fcntl.h>
int create_process(){
    pid_t pid = fork();
    if (-1 == pid){
        perror("fork");
        exit(-1);
    }
    return pid;
}

int main(){
    std::cout << "Enter file name" << '\n';
    std::string filename;
    std::cin >> filename;
    
    int pipe_fd[2];
    int err = pipe(pipe_fd);
    if (err == -1){
        perror("pipe");
        return -1;
    }
    pid_t pid = create_process();
    if (0 == pid){
        // child process
        //std::cout << "hi from child" << '\n';
        close(pipe_fd[0]);
        int FD = open(filename.c_str(), O_RDONLY);
        if (FD == -1){
            close(pipe_fd[1]);
            std::cout << "Error opening file" << '\n';
            return 1;
        }
        //std::cout << "dup2 in" << '\n';
        if (dup2(FD, STDIN_FILENO) == -1){
            std::cout << "Error dup2" << '\n';
            return 1;
        } // in -> file
        close(FD);

        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1){
            std::cout << "Error dup2" << '\n';
            return 1;
        } // output -> pipe
        close(pipe_fd[1]);
        //std::cout << "child try to exec" << '\n';
        // run child
        //char* args[]= {"strace", "-o", "log.txt", "./child", }
        if (execl("./child", "child", NULL) == -1){
            perror("exec error");
            return -1;
        }
        
    }
    else{
        // parent process
        close(pipe_fd[1]);
        
        //std::cout << "hi from parent" << '\n';
        char buffer[256];
        int bytesRead;
        while (bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))){
            if (bytesRead != sizeof(buffer)){
                perror("error in parent read result");
                return -1;
            }
            std::cout.write(buffer, bytesRead);
        }
        close(pipe_fd[0]);
        wait(NULL);
    }
    return 0;
}
