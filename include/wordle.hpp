#include <iostream>
#include "leaderboard.hpp"
#include <fstream>
#include <random>
#include <array>
#include <unordered_set>
#include "keyboard.hpp"
#include <unordered_map>
#include "timer.hpp"
#include <exception>
#include "textBox.hpp"
#include "network.hpp"
#include <chrono>
#include "settings.hpp"
#include <thread>
#include "post.hpp"
#include "config.hpp"
#include "gameState.hpp"
#include "view.hpp"
#include "profile.hpp"
#include "utils.hpp"
class Wordly {
    private :
    std::unordered_set<std::string> dictionary;
    std::vector<std::string> rs;
    std::istream & ss;
    Keyboard keyboard;
    ViewContext view;
    GameState gameState;
    std::optional<ParserJSON> cachedDistribution;
    float timer = 0.0f;
   bool pendingGameOver = false;
    float botTimer = 0.0f;
    Profile user;
    Leaderboard leaderboard;
    Pvp pvp;
    bool openSettings = false;
    Settings settings;
    Coordinates pos = {0, 0};
    void initHistoryFile(void);
    void checkUsername(std::string & buffer);
    void getRandomWord(void);
    void getRandomWordFromServer(void);
    void getRandomWordDayChallenge(void);
    void writeKey(void);
    void parseFile(void);
    void handlePvpState(void);
    void handleLeaderboardState(void);
    std::string generateTheMostAccurateWord(void) const;
    void clearHistory(void);
    void initHistory(void);
    void enter(void);

    

    void setUsername(void);
    void trim(std::string & text) const;
    void clearVariables(void);
     void update(void);
     void readKey(void);
     void updateKeyStatus(void);
     void updatePvp(void);
     size_t calculateXpDistribution(void) const;
    public :
     bool wordChecker(void);

    Wordly(std::istream & s); 


void autoBotPlay(void);
void updateCurrentWord(const char & c);

void backspace(void);
void play(void);

bool wordCheckerFromServer(const std::string & toCheck);
};