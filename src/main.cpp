
#include <string>
#include <iostream>
#include "Interface/gui.hpp"
using namespace std;
int main(){
    std::string input;
    std::getline(std::cin, input);
    s21::Gui gui;
    while (input != "quit"){
        gui.parseRequest(input);
        std::getline(std::cin, input);
    }
    return 0;
}