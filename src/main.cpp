
#include "Interface/gui.hpp"
#include <iostream>
#include <string>
using namespace std;
int main() {
  std::string input;
  std::getline(std::cin, input);
  s21::Gui gui;
  while (input != "quit") {
    gui.parseRequest(input);
    std::getline(std::cin, input);
  }
  return 0;
}