#include <stddef.h>
#include <string>
struct Profile {
    std::string username;
    size_t totalXp = 0;
    std::pair<bool, long> dailyChallenge;
    ParserJSON usersHistory;
};