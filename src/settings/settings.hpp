#include "raylib.h"
#include "../draw/button.hpp"
#include <functional>
class Settings {
    public:
    bool hardMode;
    std::function<void()> onState;
    std::function<void()> onClose;
    Settings() : hardMode(false) {}

    void drawSettings(void);
};