#include "controller.hpp"
#include "parser.hpp"
#include <fstream>
using namespace s21;

absl::StatusOr<string> Controller::executeCommand(Command c,
                                                  const vector<string> &args) {
  storage->PurgeExpired();
  switch (c) {
  case SET: {
    if (args.size() != 6 && args.size() != 7) {
      return absl::InvalidArgumentError(
          "SET expects a key, five fields, and optional EX seconds");
    }
    const string &key = args[0];
    vector<string> userParams(args.begin() + 1, args.begin() + 6);
    absl::StatusOr<User> user = User::Create(userParams);
    if (!user.ok())
      return user.status();

    optional<TimePoint> ttl;
    if (args.size() == 7) {
      int ttlSeconds;
      if (!absl::SimpleAtoi(args[6], &ttlSeconds) || ttlSeconds < 0) {
        return absl::InvalidArgumentError(
            "TTL must be a non-negative integer number of seconds");
      }
      ttl = std::chrono::system_clock::now() + std::chrono::seconds(ttlSeconds);
    }
    if (!storage->set(key, user.value(), ttl)) {
      return absl::AlreadyExistsError("Key already exists");
    }
    return "OK!";
  }
  case GET: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("GET expects one key");
    absl::StatusOr<User> result = storage->get(args[0]);
    if (!result.ok())
      return result.status();
    return result->lastName + " " + result->firstName + " " +
           std::to_string(result->yearOfBirth) + " " + result->city + " " +
           std::to_string(result->coinsQuantity);
  }
  case EXISTS: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("EXISTS expects one key");
    return to_string(storage->exists(args[0]));
  }
  case DEL: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("DEL expects one key");
    return storage->del(args[0]) ? "OK!" : "NOT FOUND";
  }
  case UPDATE: {
    if (args.size() != 6)
      return absl::InvalidArgumentError("UPDATE expects a key and five fields");
    const string &key = args[0];
    vector<string> filterParams(args.begin() + 1, args.end());
    absl::StatusOr<User_Filter> filter = User_Filter::Create(filterParams);
    if (!filter.ok())
      return filter.status();
    Node<User> *node = storage->getNode(key);
    if (node == nullptr)
      return "NOT FOUND";
    User updated = node->getValue();
    updated = filter.value();
    node->setValue(updated);
    return "OK!";
  }
  case KEYS: {
    if (!args.empty())
      return absl::InvalidArgumentError("KEYS expects no arguments");
    vector<string> keys = storage->keys();
    if (keys.empty())
      return "NO KEYS";
    string result;
    for (const auto &key : keys)
      result += key + '\n';
    return result;
  }
  case RENAME: {
    if (args.size() != 2)
      return absl::InvalidArgumentError(
          "RENAME expects an old key and a new key");
    absl::StatusOr<bool> result = storage->rename(args[0], args[1]);
    if (!result.ok())
      return result.status();
    return "OK!";
  }
  case TTL: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("TTL expects one key");
    absl::StatusOr<int> result = storage->TTL(args[0]);
    if (!result.ok())
      return result.status();
    return to_string(result.value());
  }
  case FIND: {
    if (args.size() != 5)
      return absl::InvalidArgumentError("FIND expects five filter fields");
    absl::StatusOr<User_Filter> filter = User_Filter::Create(args);
    if (!filter.ok())
      return filter.status();
    vector<string> keys = storage->find(filter.value());
    if (keys.empty())
      return "NO RESULTS";
    string result;
    int index = 1;
    for (const auto &key : keys)
      result += to_string(index++) + ")" + key + '\n';
    return result;
  }
  case SHOWALL: {
    if (!args.empty())
      return absl::InvalidArgumentError("SHOWALL expects no arguments");
    vector<User> vals = storage->showall();
    if (vals.empty())
      return "NO RECORDS";
    string result;
    for (size_t i = 0; i < vals.size(); ++i) {
      const auto &v = vals[i];
      char buf[256];
      std::snprintf(buf, sizeof(buf),
                    "%-2zu | %-9s | %-14s | %-4d | %-7s | %-15d |\n", i + 1,
                    v.lastName.c_str(), v.firstName.c_str(), v.yearOfBirth,
                    v.city.c_str(), v.coinsQuantity);
      result += buf;
    }
    return result;
  }
  case UPLOAD: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("UPLOAD expects one file path");
    std::ifstream file(args[0]);
    if (!file.is_open())
      return absl::NotFoundError("Could not open file");

    int uploaded = 0;
    string line;
    while (std::getline(file, line)) {
      auto params = Parser::parseLine(line);
      auto userParams = std::vector<string>(params.begin() + 1, params.end());
      absl::StatusOr<User> user = User::Create(userParams);
      if (!user.ok())
        continue; // игнорируем бракованные строки
      if (storage->set(params[0], user.value()))
        ++uploaded;
    }
    return "Uploaded " + to_string(uploaded) + " records";
  }
  case EXPORT: {
    if (args.size() != 1)
      return absl::InvalidArgumentError("EXPORT expects one file path");
    std::ofstream file(args[0]);
    if (!file.is_open())
      return absl::NotFoundError("Could not open file");

    int exported = 0;
    for (const auto &key : storage->keys()) {
      absl::StatusOr<User> user = storage->get(key);
      if (!user.ok())
        return user.status();

      file << key << ' ' << user->lastName << ' ' << user->firstName << ' '
           << user->yearOfBirth << ' ' << user->city << ' '
           << user->coinsQuantity << '\n';
      if (!file)
        return absl::InternalError("Failed to write export file");
      ++exported;
    }
    return "Exported " + to_string(exported) + " records";
  }
  default:
    return "NOT SUPPORTED YET";
  }
}