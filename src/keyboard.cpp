#include "keyboard.hpp"
#include <iostream>
bool Key::clickStatus(void) const
{
    Button btn;
    btn.btn = box;
    return btn.checkClick(GetMousePosition());
}

void Keyboard::init(void) {
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

void Keyboard::updateKeyboard(const char c, Keyboard_t status) {
    auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c;
                });

                if(it != keyboard.end()) {
                    it->status = status;
                }
}