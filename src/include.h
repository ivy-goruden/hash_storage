#ifndef MAIN_INCLUDE
#define MAIN_INCLUDE
#include <map>
#include <string>
using namespace std;
enum Command{
    SET,
    GET,
    EXISTS,
    DEL,
    UPDATE,
    KEYS,
    RENAME,
    TTL,
    FIND,
    SHOWALL,
    UPLOAD,
    EXPORT
};

static map<string, Command> commandsMap = {
    {"SET",SET},
    {"GET", GET},
    {"EXISTS", EXISTS},
    {"DEL", DEL},
    {"UPDATE", UPDATE},
    {"KEYS", KEYS},
    {"RENAME", RENAME},
    {"TTL", TTL},
    {"FIND", FIND},
    {"SHOWALL", SHOWALL},
    {"UPLOAD", UPLOAD},
    {"EXPORT", EXPORT}
};

#endif