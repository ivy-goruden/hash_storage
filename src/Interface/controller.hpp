#ifndef CONTROLLER
#define CONTROLLER
#include "../include.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <string>
#include "../classes/hash_map.hpp"
#include "../classes/red_black_tree.hpp"
#include "../classes/storage.hpp"
#include "absl/strings/numbers.h"
#include <cstdio>
#include "user.hpp"
#include <memory>
namespace s21{

class Controller{
    public:
        Controller(std::unique_ptr<Storage<User>> strg){
            storage = std::move(strg);
        }
        ~Controller(){}
        absl::StatusOr<string> executeCommand(Command c, const vector<string>& args);
    private:
        std::unique_ptr<Storage<User>> storage;
};

}
#endif