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
        for(const auto &c : history[activeY]) {
            if(!isalpha(c.c)) return false;
        }

        return true;
    }
    Color Wordly::getColor(const Type & t)const {
        switch(t) {
            case INCORRECT_POS :
            return YELLOW;

            case CORRECT_POS :
            return GREEN;

            default: 
            return GRAY;
        }
        return GRAY;
    }
    void Wordly::clearHistory(void) {
        std::for_each(history.begin(), history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    size_t Wordly::getLength(const std::string & str) const {return str.length();}
    

      bool Wordly::wordChecker(void) {
            std::string toCheck;
         for(const auto & c : history[activeY]) {
            toCheck += c.c;
         }
         if(toCheck.find(" ") != std::string::npos) {
            shakeTimer = 0.5f;
            return false;
         }
         if(settings.hardMode) {
            for(const char &c : this->mustUsedChars) {
                if(toCheck.find(c) == std::string::npos) {
                    renderErrorMessage = true;
                    errorMessage = "You must use ";
                    errorMessage += "letter ";
                    errorMessage += c;
                    errorMessage += " in your word";
                    shakeTimer = 0.5f;
                    return false;
                }
            }
         }
         if(!dictionary.contains(toCheck)) {
            errorMessage = "This word does not exists in our database";
            renderErrorMessage = true;
            shakeTimer = 0.5f;
            return false;
         }
         renderErrorMessage = false;
         updateKeyStatus();

        if(state == AUTOPLAY && (toCheck == word || attempts == 6)) {
            pendingGameOver = true;
            timer = 2.0f;
        }
        else if(toCheck == word) {
          
            try {
              if(state == DAILY_CHALLENGE) updateDailyChallengeStatus();
            auto current = usersHistory.getValue<int>("current_streak");
            usersHistory.updateValue<std::string>("current_streak", std::to_string(current.value() + 1));
            auto best = usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() + 1 > best.value()) {
                    usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value() + 1));
                }
            }
            auto x  = usersHistory.getValue<int>("total_games");
            auto y = usersHistory.getValue<int>("wins");
            if(x.has_value()) {
                usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                usersHistory.updateValue<std::string>("wins",std::to_string(y.value() + 1));
            }
            auto guessDistribution = usersHistory.getObject("guess_distribution");
            if(guessDistribution.has_value()) {
                auto currentValue =  guessDistribution.value().getValue<int>(std::to_string(attempts));
                if(currentValue.has_value()) {
                    guessDistribution.value().updateValue<std::string>(std::to_string(attempts), std::to_string(currentValue.value() + 1));
                    usersHistory.updateValue<std::string>("guess_distribution", guessDistribution->toString());
                }
                
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            usersHistory.stringify("../history.json");

        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
    } 
        else if(attempts == 6) {
            try {
                if(state == DAILY_CHALLENGE) updateDailyChallengeStatus();
            auto x  = usersHistory.getValue<int>("total_games");
            auto y = usersHistory.getValue<int>("losses");
            auto current = usersHistory.getValue<int>("current_streak");
            usersHistory.updateValue<std::string>("current_streak", "0");
            auto best = usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() > best.value()) {
                    usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value()));
                }

            }
            if(x.has_value()) {
                usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                usersHistory.updateValue<std::string>("losses",std::to_string(y.value() + 1));
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            usersHistory.stringify("../history.json");
        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
        }
        userWon = toCheck == word;
        return toCheck == word;
    }

void Wordly::updateCurrentWord(const char & c) {
if(activeX < 5 && activeY < 6) {
history[activeY][activeX] = Character(c, NOT_IN);
activeX++;
}

}

void Wordly::backspace(void) {
    renderErrorMessage = false;
    errorMessage.clear();
    if(activeX > 0) {
        activeX--;
        history[activeY][activeX] = Character(' ', NOT_IN);
    }

}

void Wordly::enter(void) {
     if(state != PVP) {
        wordChecker();
        }
            else {
                if(!manager.isWaitingForServer) {
                std::string usersWord;
                for(const auto & x : history[activeY]) {
                    usersWord += x.c;
                }
                if(usersWord.length() != 5) {
                    shakeTimer = 0.5f;
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
            if(!errorMessage.empty()) {
                    errorMessage.clear();
                    renderErrorMessage = false;
                }
        if(x.c == "DEL") {
            if(activeX > 0) {
                history[activeY][--activeX].c = ' ';
            }
        }
        else if(x.c == "ENT") {
            if(activeX == 5) {
                enter();
            } else shakeTimer = 0.5f;
        }
        else if(activeX < 5) {
            history[activeY][activeX++].c = x.c[0];
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
    this->word = rs[id % rs.size()];
}



void Wordly::clearVariables(void) {
                gameOver = false;
                clearHistory();
                activeX = 0;
                activeY = 0;
                attempts = 0;
                errorMessage = "";
                renderErrorMessage = false;
                leaderboard.leaderboardUpdated = false;
                leaderboard.leaderboardLoaded = false;
                mustUsedChars.clear();
                initKeyboard();
}


void Wordly::readKey(void){
    if(state == AUTOPLAY) {
        } else {
        int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            updateCurrentWord((char) key);
            if(renderErrorMessage || manager.packet.error) {
                    manager.packet.error = false;
                    errorMessage.clear();
                    renderErrorMessage = false;

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
    return static_cast<size_t>(1000 / attempts);
}

