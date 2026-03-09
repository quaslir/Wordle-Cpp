#include <iostream>
#include "leaderboard/leaderboard.hpp"
#include <fstream>
#include <random>
#include <array>
#include <unordered_set>
#include "keyboard.hpp"
#include <unordered_map>
#include "timer.hpp"
#include <exception>
#include "textBox.hpp"
#include "./pvp/network.hpp"
#include <chrono>
#include "settings/settings.hpp"
#include <thread>
#include "post.hpp"
#define SQUARE_SIZE 65
#define FONT_SIZE 30
#include "gameState.hpp"
#include "view.hpp"
#include "profile.hpp"

struct Coordinates {
    int x, y;
};



class Wordly {
    private :
    std::unordered_set<std::string> dictionary;
    std::vector<std::string> rs;
    std::istream & ss;
    std::vector<Key> keyboard;
    ViewContext view;
    GameState gameState;
    std::optional<ParserJSON> cachedDistribution;
    float timer = 0.0f;
   bool pendingGameOver = false;
    float botTimer = 0.0f;
    Timer mainTimer;
    Profile user;
    Leaderboard leaderboard;
    NetworkManager manager;
    bool openSettings = false;
    Settings settings;
    Coordinates pos = {0, 0};
    bool isEmpty(std::string_view str) const;
    void initHistoryFile(void);
    bool handleInput(std::string_view word) const;
    void updateDailyChallengeStatus(void);
    void checkUsername(std::string & buffer);
        void initKeyboard(void);
    int centerTextByX(const std::string & text, int fontSize, int width, int marginX) const;
    void getRandomWord(void);
    void getRandomWordFromServer(void);
    void getRandomWordDayChallenge(void);
    void writeKey(void);
    void parseFile(void);
    void renderKeyBoard(void) ;
    bool lengthChecker(void) const;
    std::string generateTheMostAccurateWord(void) const;
    Color getColor(const Type & t)const;
    void clearHistory(void);
    long generateDayId(void) const;
    size_t getLength(const std::string & str) const;
    void initHistory(void);
    void enter(void);

    
    ParserJSON usersHistory;

    void setUsername(void);
    void trim(std::string & text) const;
    void clearVariables(void);
    void drawOriginalStateGame(void);
     void update(void);
     void drawPvp(void);
     void drawGrid(const float offset);
     void readKey(void);
     void updateKeyStatus(void);
     void updatePvp(void);
     void wordCheckerHelpFunctionForError(void);
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