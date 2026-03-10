#include "button.hpp"
#include <vector>
#include "raylib.h"
#pragma once
enum Keyboard_t {NOT_CHECKED, INVALID, CORRECT, INCORRECT};
struct Key {
        std::string c;
        Keyboard_t status;
        Rectangle box;
        bool clickStatus(void) const ;
    };

    struct Keyboard {
        std::vector<Key> keyboard;

        void init(void);
        void updateKeyboard(const char c, Keyboard_t status);
    };
