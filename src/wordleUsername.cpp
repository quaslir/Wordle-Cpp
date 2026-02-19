#include "wordle.hpp" 
void Wordly::trim(std::string & text) const {
text.erase(0, text.find_first_not_of(" \t\r\n"));
text.erase(text.find_last_not_of(" \t\r\n") + 1);
}
void Wordly::setUsername(void) {
    static std::string buffer{""};
    Rectangle rec = {170, 300, 180, 30};
    Rectangle rec2 = rec;
    rec2.y += rec.height + 15;
    drawTextBox(rec, 2.0f, DARKGRAY, 20, buffer);
    static Color color = PURPLE;
    Button btn = drawBtn(rec2, "Set username", color);
    if(btn.checkHover(GetMousePosition())) color = DARKPURPLE;
    else color = PURPLE;
    if(btn.checkClick(GetMousePosition())) {
        trim(buffer);
            if(buffer.empty()) return;
            this->username = buffer;
            state = MAIN_MENU;
            try {
               usersHistory.updateValue<std::string>("username", username);
               usersHistory.stringify();
            } catch(...) {
                std::cerr << "Json data was corrupted, could not update username" << std::endl;
            }
            buffer.clear();
    }
    int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            buffer += (char) key;
                
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            if(buffer.size() > 0) {
                buffer.pop_back();
            }
        }

        if(IsKeyPressed(KEY_ENTER)) {
            trim(buffer);
            if(buffer.empty()) return;
            this->username = buffer;
            state = MAIN_MENU;
            try {
               usersHistory.updateValue<std::string>("username", username);
               usersHistory.stringify();
            } catch(...) {
                std::cerr << "Json data was corrupted, could not update username" << std::endl;
            }
            buffer.clear();
        }

}