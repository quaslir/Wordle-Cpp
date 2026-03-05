#include "../wordle.hpp"

bool Wordly::getAutoplayStatus(void) const {
    return state == AUTOPLAY;
}
std::string Wordly::generateTheMostAccurateWord(void) const {
std::string notInWord;
std::unordered_map<int, char> incorrectPosition;
std::unordered_map<int, char> correct;
std::vector<std::string> total;
std::string buffer;
for(const auto & row : history) {

        buffer.clear();
    for(int i = 0; i < row.size(); i++) {
        buffer += row[i].c;
        if(row[i].type == NOT_IN) {
            bool needed = false;
            for(const auto & cell : row) {
                if((cell.c == row[i].c) && (cell.type == INCORRECT_POS || cell.type == CORRECT_POS)) needed = true;
            }
            if(notInWord.find(row[i].c) == std::string::npos &&!needed ) {
                 notInWord += row[i].c;
            }

           
        }
        else if(row[i].type == INCORRECT_POS) {
            if(!incorrectPosition.contains(row[i].c)) {
            incorrectPosition.insert({i, row[i].c});
        }
    }
        else if(row[i].type == CORRECT_POS) {
           correct.insert({i, row[i].c});
        }
    }
    total.push_back(buffer);
}

auto mostProbableWord = std::find_if(rs.begin(), rs.end(), [notInWord, incorrectPosition, correct, total] (const std::string &w) {
    for(const auto c : total) {
        if(w == c) return false;
    }
for(const char & c : notInWord) {
    if(w.find(c) != std::string::npos) return false;
}

for(const auto & c : incorrectPosition) {
    if(w.find(c.second) == std::string::npos || w[c.first] == c.second) return false;
}

for(auto & c : correct) {
    if(w[c.first] != c.second) return false;
}

return true;

});
if(mostProbableWord != rs.end())  return *mostProbableWord;
return "";
}
void Wordly::autoBotPlay(void) {
    if(activeY == 6 || pendingGameOver) return;
    if(botTimer >= 0) {
        botTimer -= GetFrameTime();
        return;
    }
    std::string target;
    if(activeY ==0) {
        target = rs[1];
    }
    else {
target = generateTheMostAccurateWord();
    }
    std::uniform_int_distribution<> dis(0, rs.size());
    if(activeX == 0) {
    for(int i = 0; i < 5; i++) {
    history[activeY][activeX++].c = target[i];  
    }
    if(wordChecker());
    else botTimer = 0.8f;
}
}