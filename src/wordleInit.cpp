#include "wordle.hpp"
std::random_device rd;
std::mt19937 gen(rd());

Wordly::Wordly(std::istream & s) : ss(s) {
   gameState.state = MAIN_MENU;
    user.initHistoryFile(0);
     user.initHistory();
      this->parseFile();  
    if(user.usersHistory.exists("username")) {
        user.username = user.usersHistory.getValue<std::string>("username").value();
        if(user.username.empty()) gameState.state = EMPTY_USERNAME;
        trim(user.username);
        if(user.username.empty()) gameState.state = EMPTY_USERNAME;
    }
    else gameState.state = EMPTY_USERNAME;

    if(gameState.state != EMPTY_USERNAME) {
        leaderboard.getXp = [this](int xp) {
        user.totalXp = xp;
       };
       std::thread([this] {
        leaderboard.receiveUsersXp(user.username);
    }).detach();
    }
       
       if(user.usersHistory.exists("daily_challenge")) {

        auto x = user.usersHistory.getObject("daily_challenge");
        if(x.has_value()) {
                            
        auto first = x.value().getValue<bool>("daily_challenge_active");
        auto second = x.value().getValue<long>("daily_challenge_id");
        if(first.has_value() && second.has_value()) {

            user.dailyChallenge.first = first.value();
            user.dailyChallenge.second = second.value();
            if(!user.dailyChallenge.first) {
                long current = utils::generateDayId();
                if(current != user.dailyChallenge.second) {
                    user.dailyChallenge.first = true;
                    user.dailyChallenge.second = current;
                    user.usersHistory.stringify("../history.json");
                }
            }
        }
    }
       }
       keyboard.init();
           
       leaderboard.changeState = [this]() {
        gameState.state = MAIN_MENU;
       };
       
       leaderboard.getUsername = [this]() {
        return user.username;
       };
       pvp.setOnWord([this] (const std::string & word) {
        for(int i = 0; i < 5; i++) {
        gameState.history[view.activeY][view.activeX++].c = word[i];  
    }
    updateKeyStatus();
       });

    settings.onState = [this] () {
        if(gameState.state == PVP) {
            pvp.disconnect();
        }
        gameState.state = MAIN_MENU;
        clearVariables();
    };

    settings.onClose = [this] () {
        openSettings = false;
    };

    view.onDailyChallenge = [this]() {
        return user.dailyChallenge.first;
    };

    view.onClick = [this] (int i) {
        switch (i) {
            case 0 :    
            if(view.onDailyChallenge()) {
                gameState.state = DAILY_CHALLENGE; 
                gameState.mainTimer.start();
                getRandomWordDayChallenge();
                user.dailyChallenge.second = utils::generateDayId();
            }
                break;
            case 1 :
                gameState.state = PRACTICE;
                gameState.mainTimer.start();
                getRandomWord();
                std::cout << gameState.word << std::endl;

                break;
            case 2 :
            gameState.state = AUTOPLAY;
            gameState.mainTimer.start();
            getRandomWord();
            break;

            case 3:
            if(settings.offlineMode) break;
            gameState.state = LEADERBOARD;
            break;

            case 4:
            if(settings.offlineMode) break;
            gameState.state = PVP;
            pvp.connect("ws://localhost:8000");
            break;

            case 5: 
            std::exit(0);
        }
    };

    view.getUsername = [this] () {
        return user.username;
    };

    view.onSettings = [this] () {
        openSettings = !openSettings;
    };

    view.onPlayAgain = [this] () {
        clearVariables();
        getRandomWord();
        gameState.mainTimer.start();
    };

    view.onExit = [this] () {
        clearVariables();
        gameState.state = MAIN_MENU;
    };

    view.getAttempts = [this] () {
        return gameState.attempts;
    };

    view.getUserWon = [this] () {
        return gameState.userWon;
    };

    view.getTotalXp = [this] () {
        return user.totalXp;
    };

    view.getWord = [this] () {
        return gameState.word;
    };

    view.getMins = [this] () {
        return gameState.mainTimer.getMins();
    };

    view.getSeconds = [this] () {
        return gameState.mainTimer.getSeconds();
    };

    view.getDistribution = [this] () -> const std::optional<ParserJSON> &{
        cachedDistribution = user.usersHistory.getObject("guess_distribution");
        return cachedDistribution;
    };
    view.getValue = [this](const std::string &key) {
        return user.usersHistory.getValue<int>(key);
    };

    view.getPos = [this] () {
        return pos;
    };

    view.getKeyboard = [this] () -> const std::vector<Key>&{
        return this->keyboard.keyboard;
    };

    view.setKey = [this] (const Rectangle & rec, Key & key) {
        key.box = rec;
    };

    view.getChar = [this] (int x, int y) {
        if(x < 5 && x >= 0 && y < 6 && y >= 0) {
            return gameState.history[y][x];
        }

        return Character(' ', NOT_IN);
    };

    view.getPvpStatus = [this](void) {
        return pvp.getStatus();
    };

    view.onBackBtn = [this] (void) {
        pvp.disconnect();
         clearVariables();
         leaderboard.leaderboardUpdated = false;
        gameState.state = MAIN_MENU;
    };

    view.getGameOver = [this](void) {
        return gameState.gameOver;
    };
    view.getOpenSettings = [this](void) {
        return openSettings;
    };

    view.onHint = [this] (void) {
        if(!view.renderErrorMessage && !hint.hintLoaded && (gameState.state == DAILY_CHALLENGE || gameState.state == PRACTICE)) {
        hint.drawHintBtn();
        }
    };

    view.onPractice = [this](void) {
        return gameState.state == PRACTICE;
    };

    user.getAttempts = [this] (void) {
        return gameState.attempts;
    };

    leaderboard.setOfflineState = [this] () {
        settings.offlineMode = true;
    };
    user.getDailyChallengeState = [this] () {
        return gameState.state == DAILY_CHALLENGE;
    };
}

void Profile::initHistoryFile(bool force) {
    std::ifstream historyCheck("../history.json", std::ios::out | std::ios::binary);
        if(historyCheck.is_open() && !force) return;      
    std::ofstream history("../history.json", std::ios::out | std::ios::binary);
    ParserJSON main (true, "hello");
    main.insert("total_games", 0);
    main.insert("best_streak", 0);
    main.insert("last_played_date", "");
    main.insert("losses", 0);
    main.insert("guess_distribution", "");
    main.insert("current_streak", 0);
    main.insert("wins", 0);
    main.insert("username", "");
    main.insert("daily_challenge", "");
    ParserJSON dailyCh;
    dailyCh.insert("daily_challenge_active", "false");
    dailyCh.insert("daily_challenge_id", 0);
    main.updateValue<std::string>("daily_challenge", dailyCh.toString());
    ParserJSON submain;
    submain.insert("1", 0);
    submain.insert("2", 0);
    submain.insert("3", 0);
    submain.insert("4", 0);
    submain.insert("5", 0);
    submain.insert("6", 0);

    main.updateValue<std::string>("guess_distribution", submain.toString());
    main.stringify("../history.json");
}


void Profile::initHistory(void) {
        std::ifstream file ("../history.json", std::ios::out | std::ios::binary);
        try {
        usersHistory.parse(file);
        } catch(const std::exception & error) {
            std::cerr << error.what() << std::endl;
            initHistoryFile(1);
            usersHistory.clear();

        }
    }

    void Wordly::getRandomWord(void)  {
        if(!settings.offlineMode) {
            getRandomWordFromServer();
            return;
        }
        std::uniform_int_distribution<> dis(0, rs.size() - 1);
        gameState.word = rs[dis(gen)];
    }