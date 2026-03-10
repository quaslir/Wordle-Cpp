#include "wordle.hpp"

        void Wordly::parseFile(void) {
        std::string buffer;
        while(getline(ss, buffer)) {
            if(utils::isEmpty(buffer)) continue;

            rs.push_back(buffer);
            dictionary.insert(buffer);
         }

    }

    void Wordly::clearHistory(void) {
        std::for_each(gameState.history.begin(), gameState.history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    
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
        void Profile::updateCommonStats(void) {
            auto x  = usersHistory.getValue<int>("total_games");

        if(x.has_value()) {
                usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            auto current = usersHistory.getValue<int>("current_streak");
            auto best = usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() > best.value()) {
                    usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value()));
                }

            }
        }
        void Profile::updateUsersStatsWin(void) {
            auto current = usersHistory.getValue<int>("current_streak");
            usersHistory.updateValue<std::string>("current_streak", std::to_string(current.value() + 1));
            auto y = usersHistory.getValue<int>("wins");
            if(y.has_value()) {
                usersHistory.updateValue<std::string>("wins",std::to_string(y.value() + 1));
            }
            auto guessDistribution = usersHistory.getObject("guess_distribution");
            if(guessDistribution.has_value()) {
                auto currentValue =  guessDistribution.value().getValue<int>(std::to_string(getAttempts()));
                if(currentValue.has_value()) {
                    guessDistribution.value().updateValue<std::string>(std::to_string(getAttempts()), std::to_string(currentValue.value() + 1));
                    usersHistory.updateValue<std::string>("guess_distribution", guessDistribution->toString());
                }
                
            }
            usersHistory.stringify("../history.json");
        }

        void Profile::updateUsersStatsLose(void) {
            
            auto y = usersHistory.getValue<int>("losses");
            usersHistory.updateValue<std::string>("current_streak", "0");

            if(y.has_value()) {
                usersHistory.updateValue<std::string>("losses",std::to_string(y.value() + 1));
            }
            usersHistory.stringify("../history.json");
            
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
              if(gameState.state == DAILY_CHALLENGE) user.updateDailyChallengeStatus();
            user.updateUsersStatsWin();
            pendingGameOver = true;
            timer = 2.0f;

    } 
        else if(gameState.attempts == 6) {
            if(gameState.state == DAILY_CHALLENGE) user.updateDailyChallengeStatus();
            user.updateUsersStatsLose();
            pendingGameOver = true;
            timer = 2.0f;
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
for(const auto & x : keyboard.keyboard) {
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

void Wordly::getRandomWordDayChallenge(void) {
    long id = utils::generateDayId();
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
                keyboard.init();
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