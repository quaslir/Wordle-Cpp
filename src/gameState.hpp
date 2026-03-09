#include <string>
#include <unordered_set>
enum gameState_t {
    MAIN_MENU,
    AUTOPLAY,
    DAILY_CHALLENGE,
    PRACTICE,
    EMPTY_USERNAME,
    LEADERBOARD,
    PVP
};
enum Type {CORRECT_POS, INCORRECT_POS, NOT_IN};
struct Character {
    char c;
Type type;
Character(char ch, Type t) : c(ch),type(t) {}
Character() : c(' '), type(NOT_IN) {}
};


struct GameState {
    std::string word;
    size_t attempts = 0;
    std::array<std::array<Character, 5>, 6> history;
    std::unordered_set<char> mustUsedChars;
    bool userWon = false;
    bool gameOver = false;
    gameState_t state;

};