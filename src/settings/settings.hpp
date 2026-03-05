#include "raylib.h"
#include "../draw/button.hpp"
class Settings {
    public:
    bool hardMode;
    
    Settings() : hardMode(false) {}

    void drawSettings(void);
};