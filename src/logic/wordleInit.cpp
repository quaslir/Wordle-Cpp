#include "../wordle.hpp"
std::random_device rd;
std::mt19937 gen(rd());

Wordly::Wordly(std::istream & s) : ss(s) {
    this->state = MAIN_MENU;
    this->initHistoryFile();
     this->initHistory();
    if(usersHistory.exists("username")) {
        username = usersHistory.getValue<std::string>("username").value();
        if(username.empty()) this->state = EMPTY_USERNAME;
        trim(username);
        if(username.empty()) this->state = EMPTY_USERNAME;
    }
    else this->state = EMPTY_USERNAME;

    if(state != EMPTY_USERNAME) {
        leaderboard.getXp = [this](int xp) {
        this->totalXp = xp;
       };
        leaderboard.receiveUsersXp(username);
    }
        this->parseFile();  
       if(usersHistory.exists("daily_challenge")) {

        auto x = usersHistory.getObject("daily_challenge");
        if(x.has_value()) {
                            
        auto first = x.value().getValue<bool>("daily_challenge_active");
        auto second = x.value().getValue<long>("daily_challenge_id");
        if(first.has_value() && second.has_value()) {

            dailyChallenge.first = first.value();
            dailyChallenge.second = second.value();
            if(!dailyChallenge.first) {
                long current = generateDayId();
                if(current != dailyChallenge.second) {
                    dailyChallenge.first = true;
                    dailyChallenge.second = current;
                    usersHistory.stringify("../history.json");
                }
            }
        }
    }
       }
        this->initKeyboard();
           
       leaderboard.changeState = [this]() {
        this->state = MAIN_MENU;
       };
       
       leaderboard.getUsername = [this]() {
        return this->username;
       };
       manager.setOnWord([this] (const std::string & word) {
        for(int i = 0; i < 5; i++) {
        history[activeY][activeX++].c = word[i];  
    }
    updateKeyStatus();
       });

    settings.onState = [this] () {
        state = MAIN_MENU;
    };

    settings.onClose = [this] () {
        openSettings = false;
    };
}

void Wordly::initHistoryFile(void) {

    std::ifstream historyCheck("../history.json");
        if(historyCheck.is_open()) return;      
        std::ofstream history("../history.json");
    ParserJSON main;
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


void Wordly::initKeyboard(void) {
keyboard.clear();
std::string layout{"qwertyuiopasdfghjklDELzxcvbnmENT"};
std::string buffer;
for(int i = 0; i < layout.size(); i++) {
    buffer.clear();
    if(layout[i] == 'D') {
        while(layout[i] != 'z') buffer += layout[i++];
    }
    else if(layout[i] == 'E') {
        while(layout[i]) buffer += layout[i++];
    }
    else {
         buffer += layout[i];
    }
   
    keyboard.push_back({buffer, NOT_CHECKED});
}
}

void Wordly::initHistory(void) {
        std::ifstream file ("../history.json");
        this->usersHistory.parse(file);
    }

    void Wordly::getRandomWord(void)  {
        std::uniform_int_distribution<> dis(0, rs.size() - 1);
         this->word = rs[dis(gen)];
    }