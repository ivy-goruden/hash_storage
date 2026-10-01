
#include "Interface/gui.hpp"
#include <iostream>
#include <string>
using namespace std;
int main() {
  s21::Gui gui;
  std::string input;
  cout << "Hash Storage — консольное key-value хранилище.\n"
        "Выберите структуру хранения и вводите команды.\n"
        "Пример: SET user:1 Ivanov Ivan 1998 Moscow 120\n"
        "Для выхода введите quit.\n";
  getline(std::cin, input);
  while (input != "quit") {
    gui.parseRequest(input);
    getline(std::cin, input);
  }
  return 0;
}