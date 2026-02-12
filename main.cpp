#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "wordly.hpp"
#include "raylib.h"
#include <array>
#include <unistd.h>
#include <unordered_set>
std::random_device rd;
std::mt19937 gen(rd());
enum Type {CORRECT_POS, INCORRECT_POS, NOT_IN};

struct Character {
    char c;
Type type;

Character(char ch, Type t) : c(ch),type(t) {}
Character() : c(' '), type(NOT_IN) {}
};

struct Config {
Color bg_color;
Color grid_color;
Color text_color;
Config() : bg_color(BLACK), grid_color(GREEN), text_color(GREEN) {}
};

struct Button {
    Rectangle btn;
    Color color;
    std::string text;
    bool isClicked;

    bool checkClick(Vector2 pos) {
        if(CheckCollisionPointRec(pos, btn)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return true;
            }
        }
        return false;
    }
    
    Button (const Rectangle & rec, const Color & c, const std::string & t) :
    btn(rec), color(c), text(t), isClicked(false) {}
};

class Wordly {
    private :
    std::vector<std::string> rs;
    std::istream & ss;
    size_t attempts = 0;
    std::array<std::array<Character, 5>, 6> history;
    int activeX = 0;
    int activeY = 0;
    const int startingCoordinate = 1;
    bool gameOver = false;
    std::string word;
    bool userWon;
    bool isEmpty(std::string_view str) const{
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
    }

    bool handleInput(std::string_view word) const{
        if(word.length() != 5) return false;

        for(const auto & x : word) {
            if(!isalpha(x)) return false;
        }
        return true;
    }


    void getRandomWord(void)  {
        std::uniform_int_distribution<> dis(0, rs.size());
         this->word = rs[dis(gen)];
    }
            void parseFile(void) {
        std::string buffer;
        while(getline(ss, buffer)) {
            if(this->isEmpty(buffer)) continue;

            rs.push_back(buffer);
         }
    }
    bool lengthChecker(void) const {
        for(const auto &c : history[activeY]) {
            if(!isalpha(c.c)) return false;
        }

        return true;
    }
    Color getColor(const Type & t)const {
        switch(t) {
            case INCORRECT_POS :
            return YELLOW;

            case CORRECT_POS :
            return GREEN;
        }
        return GRAY;
    }
    Color applyColor(std::string_view toCheck) const {
        if(toCheck == "RED") return RED;
        else if(toCheck == "BLUE") return BLUE;
        else if(toCheck == "GREEN") return GREEN;
        else if(toCheck == "YELLOW") return YELLOW;
        else if(toCheck == "BLACK") return BLACK;
        else if(toCheck == "PINK") return PINK;
        else if(toCheck == "BLUE") return BLUE;
        else if(toCheck == "VIOLET") return VIOLET;
        return BLACK;
    }
    void clearHistory(void) {
        std::for_each(history.begin(), history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    size_t getLength(const std::string & str) const {return str.length();}
    void readConfig(void) {
        std::ifstream config("../config.conf");
        if(!config.is_open()) {
            std::cout << "NOT FOUND" << std::endl;
            return;
        }
        std::string buffer;

        while(getline(config, buffer)) {
            size_t toFind;
            if((toFind = buffer.find("BG_COLOR=")) != std::string::npos){
                buffer = buffer.substr(getLength("BG_COLOR="));
                this->config.bg_color = applyColor(buffer);
            }
            else if((toFind = buffer.find("GRID_COLOR=")) != std::string::npos){
                buffer = buffer.substr(getLength("GRID_COLOR="));
                this->config.grid_color = applyColor(buffer);
            }
        }
    }
    public :
    Config config;
        bool wordChecker(void) {
            if(!lengthChecker()) return false;
            std::string toCheck;
        size_t idx = 0;
        for(auto & c : history[activeY]) {
            if(this->word.find(c.c) == std::string::npos) c.type = NOT_IN;
            else if(this->word[idx] == c.c) c.type  = CORRECT_POS;
            else c.type = INCORRECT_POS;
            idx++;
            toCheck += c.c;
        }
        activeX = 0;
        if(activeY < 6) {
             activeY++;
        }  
        attempts++;
        if(attempts == 6) {
            gameOver = true;

        }
         userWon = toCheck == word; 
        return toCheck == word;
    }
    Wordly(std::istream & s) : ss(s) {
        this->parseFile();
        this->getRandomWord();
        this->readConfig();
        std::cout << word << std::endl;
    }

 void draw(void) {
    DrawText("Wordly-C++",125,20,50,GREEN);
    if(!gameOver) {
    std::string buf;
       for(size_t i = 0; i < 6; i++) {
        for(size_t j = 0; j < 5; j++) {
            buf.clear();
            auto c = this->history[i][j];
            buf += c.c;
        float calculateX = (float) ((j * 70 * 1.1) + 70);
        float calculateY =  (float) ((i * 70 * 1.1) + 100);
        Rectangle box = {(float) calculateX,calculateY, 70, 70};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), buf.c_str(), 40.f, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(buf.c_str(), (int) textX, (int) textY, 40, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, this->config.grid_color);
       }
       }
    } else {
        gameOverScreenRenderer();
    }
}

void updateCurrentWord(const char & c) {
if(activeX < 5 && activeY < 6) {
history[activeY][activeX] = Character(c, NOT_IN);
activeX++;
}

}

void backspace(void) {
    if(activeX > 0) {
        activeX--;
        history[activeY][activeX] = Character(' ', NOT_IN);
    }

}
Button drawBtn(const Rectangle & box, const std::string & text, const Color & color) {
    Button btn (box, PINK, text);

    DrawRectangle(btn.btn.x, btn.btn.y, btn.btn.width, btn.btn.height, btn.color);
    DrawText(btn.text.c_str(),btn.btn.x + 10, btn.btn.y + 5, 20, BLACK);
    return btn;
}
void gameOverScreenRenderer(void) {
    if(userWon) {
    std::string text = "You guessed the word by " + std::to_string(attempts);
    std::string next = attempts == 1 ? " attempt" : " attempts";
    text += next;
    DrawText(text.c_str(), 25, 90, 25, this->config.text_color);   
    } else {
        std::string text = "Unfortunately, you did not guess the\nword " + word + " within 6 attempts";
        DrawText(text.c_str(), 25, 90, 23, this->config.text_color);   
    }
    Rectangle box = {120, 150, 120, 30};
    std::string text = "Play again";
    Button playAgain = drawBtn(box, text, PINK);
        if(playAgain.checkClick(GetMousePosition())) {
        gameOver = false;
        clearHistory();
        activeX = 0;
        activeY = 0;
        attempts = false;
        getRandomWord();
    }

    Rectangle box2 =  {255, 150, 120, 30};
    std::string text2 = "Exit";
    Button exit = drawBtn(box2, text2, PINK);

    if(exit.checkClick(GetMousePosition())) {
        std::exit(0);
    }
}
void setGameOver(void) {
    gameOver = true;
}
};

int main(int argc, char * argv[]) {
    std::ifstream file (argv[1]);
    Wordly wordly (file);
    InitWindow(540, 600, "Worldy-C++");
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(wordly.config.bg_color);
        wordly.draw();

        int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            wordly.updateCurrentWord((char) key);
   
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            wordly.backspace();
        }
        if(IsKeyPressed(KEY_ENTER)){
            if(wordly.wordChecker()) {
               wordly.setGameOver();
            }
        }
        EndDrawing();
}

CloseWindow();
    return 0;
} 