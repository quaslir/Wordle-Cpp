#include "raylib.h"
#include "button.hpp"
#include <functional>
class Settings {
    public:
    bool hardMode;
    bool offlineMode;
    std::function<void()> onState;
    std::function<void()> onClose;
    Settings() : hardMode(false) {}

    void drawSettings(void);
};