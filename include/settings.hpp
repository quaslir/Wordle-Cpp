#include "raylib.h"
#include "button.hpp"
#include <functional>
class Settings {
    public:
    bool hardMode;
    bool offlineMode;
    std::function<void()> onState;
    std::function<void()> onClose;
    std::function<void(const Rectangle & rec)> onSlider;
    Settings() : hardMode(false), offlineMode(false) {}

    void drawSettings(void);
};