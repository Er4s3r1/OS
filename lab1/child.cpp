#include <iostream>

int main(){
    //std::cout << "hi i was called and i will work" << '\n';
    double res;
    //std::cin >> res;
    int number;
    
    int newCommand = 1;
    while(std::cin.peek() != EOF){
        if (std::cin.peek() == '\n'){
            std::cin.ignore();
            std::cout << res << '\n';
            //std::cout << "first" << '\n';
            newCommand = 1;

        }
        if (newCommand){
            std::cin>>res;
            //std::cout << "second" << '\n';
            newCommand = 0;
        }
        if (!newCommand){
            std::cin >> number;
            //std::cout << "third" << '\n';
            if (number == 0){
            std::cout << "Division by 0" << '\n';
            return 1;
            }
            res /=number;
        }
    }
    //std::cout << res << '\n';
}
