#include <curl/curl.h>
#include <string>
#include <vector>
#include <algorithm>
#include "raylib.h"
#include <sstream>
#include "../json-parser-c/parser.hpp"
#include "drawLogo.hpp"
#include <functional>
class Leaderboard {
    public :
    std::function<void(int)> getXp;
    std::function<void()> changeState;
    std::function<std::string()> getUsername;
    bool leaderboardLoaded = false;
    bool leaderboardUpdated = false;
    std::vector<std::pair<std::string, size_t>>  leaderboard;
    void loadLeaderboard(void);
    void renderLeaderboard(void);

    void updateLeaderboard(const std::string & username, const size_t xp);
    void receiveUsersXp(const std::string & username);
};