#include "button.hpp"
enum Keyboard {NOT_CHECKED, INVALID};
struct Key {
        char c;
        Keyboard status;
        Rectangle box;
        bool clickStatus(void) const ;
    };
