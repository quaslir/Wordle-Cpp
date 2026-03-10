#include "wordle.hpp"


bool Wordly::isEmpty(std::string_view str) const{
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
}

    bool Wordly::handleInput(std::string_view word) const{
        if(word.length() != 5) return false;

        for(const auto & x : word) {
            if(!isalpha(x)) return false;
        }
        return true;
    }

            void Wordly::parseFile(void) {
        std::string buffer;
        while(getline(ss, buffer)) {
            if(isEmpty(buffer)) continue;

            rs.push_back(buffer);
            dictionary.insert(buffer);
         }

    }
    bool Wordly::lengthChecker(void) const {
        for(const auto &c : gameState.history[view.activeY]) {
            if(!isalpha(c.c)) return false;
        }

        return true;
    }
    void Wordly::clearHistory(void) {
        std::for_each(gameState.history.begin(), gameState.history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    size_t Wordly::getLength(const std::string & str) const {return str.length();}
    
        std::string GameState::handleHardMode(const std::string & toCheck) const {
            std::string error;
             for(const char &c : mustUsedChars) {
                if(toCheck.find(c) == std::string::npos) {
                    error = "You must use ";
                    error += "letter ";
                    error += c;
                    error += " in your word";
                    return error;
                }
            }
            return "";
        }
      bool Wordly::wordChecker(void) {
            std::string toCheck;
         for(const auto & c : gameState.history[view.activeY]) {
            toCheck += c.c;
         }
         if(toCheck.find(" ") != std::string::npos) {
            view.shakeTimer = 0.5f;
            return false;
         }
         if(settings.hardMode) {
           std::string error = gameState.handleHardMode(toCheck);
            if(!error.empty()) {
                view.errorMessage = error;
                view.renderErrorMessage = true;
            }
         }
         if(settings.offlineMode && !dictionary.contains(toCheck)) {
           wordCheckerHelpFunctionForError();
           return false;
         }
         else if(!settings.offlineMode && !wordCheckerFromServer(toCheck)) {
             wordCheckerHelpFunctionForError();
            return false;
         }
         view.renderErrorMessage = false;
         updateKeyStatus();

        if(gameState.state == AUTOPLAY && (toCheck == gameState.word || gameState.attempts == 6)) {
            pendingGameOver = true;
            timer = 2.0f;
        }
        else if(toCheck == gameState.word) {
          
            try {
              if(gameState.state == DAILY_CHALLENGE) updateDailyChallengeStatus();
            auto current = user.usersHistory.getValue<int>("current_streak");
            user.usersHistory.updateValue<std::string>("current_streak", std::to_string(current.value() + 1));
            auto best = user.usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() + 1 > best.value()) {
                    user.usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value() + 1));
                }
            }
            auto x  = user.usersHistory.getValue<int>("total_games");
            auto y = user.usersHistory.getValue<int>("wins");
            if(x.has_value()) {
                user.usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                user.usersHistory.updateValue<std::string>("wins",std::to_string(y.value() + 1));
            }
            auto guessDistribution = user.usersHistory.getObject("guess_distribution");
            if(guessDistribution.has_value()) {
                auto currentValue =  guessDistribution.value().getValue<int>(std::to_string(gameState.attempts));
                if(currentValue.has_value()) {
                    guessDistribution.value().updateValue<std::string>(std::to_string(gameState.attempts), std::to_string(currentValue.value() + 1));
                    user.usersHistory.updateValue<std::string>("guess_distribution", guessDistribution->toString());
                }
                
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            user.usersHistory.stringify("../history.json");

        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
    } 
        else if(gameState.attempts == 6) {
            try {
                if(gameState.state == DAILY_CHALLENGE) updateDailyChallengeStatus();
            auto x  = user.usersHistory.getValue<int>("total_games");
            auto y = user.usersHistory.getValue<int>("losses");
            auto current = user.usersHistory.getValue<int>("current_streak");
            user.usersHistory.updateValue<std::string>("current_streak", "0");
            auto best = user.usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() > best.value()) {
                    user.usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value()));
                }

            }
            if(x.has_value()) {
                user.usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                user.usersHistory.updateValue<std::string>("losses",std::to_string(y.value() + 1));
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            user.usersHistory.stringify("../history.json");
        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
        }
        gameState.userWon = toCheck == gameState.word;
        return toCheck == gameState.word;
    }

void Wordly::updateCurrentWord(const char & c) {
if(view.activeX < 5 && view.activeY < 6) {
gameState.history[view.activeY][view.activeX] = Character(c, NOT_IN);
view.activeX++;
}

}

void Wordly::backspace(void) {
    view.renderErrorMessage = false;
    view.errorMessage.clear();
    if(view.activeX > 0) {
        view.activeX--;
        gameState.history[view.activeY][view.activeX] = Character(' ', NOT_IN);
    }

}

void Wordly::enter(void) {
     if(gameState.state != PVP) {
        wordChecker();
        }
            else {
                if(!manager.isWaitingForServer) {
                std::string usersWord;
                for(const auto & x : gameState.history[view.activeY]) {
                    usersWord += x.c;
                }
                if(usersWord.length() != 5) {
                    view.shakeTimer = 0.5f;
                    return; 
                }
                manager.isWaitingForServer = true;    
                manager.sendGamePacket(usersWord);
                
            }
        }
}

void Wordly::writeKey(void) {
for(const auto & x : keyboard) {
    if(x.clickStatus()) {
            if(!view.errorMessage.empty()) {
                    view.errorMessage.clear();
                    view.renderErrorMessage = false;
                }
        if(x.c == "DEL") {
            if(view.activeX > 0) {
                gameState.history[view.activeY][--view.activeX].c = ' ';
            }
        }
        else if(x.c == "ENT") {
            if(view.activeX == 5) {
                enter();
            } else view.shakeTimer = 0.5f;
        }
        else if(view.activeX < 5) {
            gameState.history[view.activeY][view.activeX++].c = x.c[0];
        }
    }
}
}


long Wordly::generateDayId(void) const {
    auto now = std::chrono::system_clock::now();
    time_t formatted = std::chrono::system_clock::to_time_t(now);
    
    struct tm * parts = std::localtime(&formatted);

    return (parts->tm_year + 1900) * 1000 + (parts->tm_mon + 1) * 100 + parts->tm_mday;
}

void Wordly::getRandomWordDayChallenge(void) {
    long id = generateDayId();
    gameState.word = rs[id % rs.size()];
}



void Wordly::clearVariables(void) {
                gameState.gameOver = false;
                clearHistory();
                view.activeX = 0;
                view.activeY = 0;
                gameState.attempts = 0;
                view.errorMessage = "";
                view.renderErrorMessage = false;
                leaderboard.leaderboardUpdated = false;
                leaderboard.leaderboardLoaded = false;
                gameState.mustUsedChars.clear();
                initKeyboard();
}


void Wordly::readKey(void){
    if(gameState.state == AUTOPLAY) {
    } else {
        int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            updateCurrentWord((char) key);
            if(view.renderErrorMessage || manager.packet.error) {
                    manager.packet.error = false;
                    view.errorMessage.clear();
                    view.renderErrorMessage = false;

            }
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            backspace();
        }
        if(IsKeyPressed(KEY_ENTER)){
            enter();
        }
    }
}



size_t Wordly::calculateXpDistribution(void) const {
    return static_cast<size_t>(1000 / gameState.attempts);
}

bool Wordly::wordCheckerFromServer(const std::string & toCheck) {
    ParserJSON data;
    data.insert("word", toCheck);
    std::string dataFormatted = data.toString();
    std::string result = postRequest("http://localhost:3000/word-check", dataFormatted);
    data.clear();
    std::stringstream ss (result);
    data.parse(ss);

    if(data.exists("exists")) {
        auto x = data.getValue<bool>("exists");
        if(x.has_value()) {
            return x.value();
        }
    }

    return false;
}


void Wordly::getRandomWordFromServer(void) {
    std::string raw = getRequest("http://localhost:3000/word-gen");
    ParserJSON parser;
    std::stringstream ss (raw);
    parser.parse(ss);

    if(parser.exists("word")) {
        auto x  = parser.getValue<std::string>("word");
        if(x.has_value()) {
            gameState.word = x.value();
        }
    }
 }

 void Wordly::wordCheckerHelpFunctionForError(void) {
 view.errorMessage = "This word does not exists in our database";
            view.renderErrorMessage = true;
            view.shakeTimer = 0.5f;
 }