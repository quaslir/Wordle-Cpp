#include <stddef.h>
#include <string>
#include <functional>
struct Profile {
    std::string username;
    size_t totalXp = 0;
    std::pair<bool, long> dailyChallenge;
    ParserJSON usersHistory;
    std::function<size_t(void)> getAttempts;
    void updateUsersStatsWin(void);
    void updateUsersStatsLose(void);
    void updateCommonStats(void);
    void updateDailyChallengeStatus(void);
};