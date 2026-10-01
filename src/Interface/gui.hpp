#ifndef GUI_HASHER
#define GUI_HASHER
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "controller.hpp"
#include "../include.h"
#include "parser.hpp"
using namespace std;
namespace s21{
class Gui{
    private:
        std::unique_ptr<Controller> c;
    public:
        Gui(){
            string choice;
            while (true){
                std::cout << "Select storage system:\n1. Hash_Map\n2. RBTree\n> ";
                std::cin >> choice;
                if (choice == "1"){
                    c = std::make_unique<Controller>(std::make_unique<Hash_Map<User>>());
                    break;
                }
                if (choice == "2"){
                    c = std::make_unique<Controller>(std::make_unique<RedBlackTree<User>>());
                    break;
                }
                std::cout << "Invalid choice.\n";
            }
        }
        ~Gui() = default;
        void parseRequest(std::string request){
            vector<string> params = Parser::parseLine(request);
            if (params.empty()) return;
            string com_val = params[0];
            if (commandsMap.find(com_val) == commandsMap.end()){
                std::cout << "Wrong Command!" << std::endl;
                return;
            }

            Command command = commandsMap[com_val];
            auto userParams = std::vector<string>(params.begin()+1, params.end()); 
            absl::StatusOr<string> status = c->executeCommand(command, userParams);
            if (!status.ok()){
                std::cout << status.status() << std::endl;
            }
            else{
                std::cout << status.value() << std::endl;
                
            }
        }
};
}
#endif