#include <stddef.h>
#include <string>
#include <functional>
#include <unordered_map>
    struct Profile {
    std::string username;
    size_t totalXp = 0;
    std::pair<bool, long> dailyChallenge;
    ParserJSON usersHistory  = ParserJSON(true, "hello");
    std::function<size_t(void)> getAttempts;
    std::function<bool(void)> getDailyChallengeState;
    std::unordered_map<std::string, int> values;
    void updateUsersStatsWin(void);
    void updateUsersStatsLose(void);
    void updateCommonStats(void);
    void updateDailyChallengeStatus(void);
    void initHistoryFile(bool force);
    void initHistory(void);

    void updateCachedValues(void) ;
};