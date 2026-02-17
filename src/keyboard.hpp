#include "button.hpp"
enum Keyboard {NOT_CHECKED, INVALID};
struct Key {
        std::string c;
        Keyboard status;
        Rectangle box;
        bool clickStatus(void) const ;
    };
