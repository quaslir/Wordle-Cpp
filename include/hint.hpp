#include "parser.hpp"
#include "raylib.h"
#include <string>
#include <sstream>
#include "post.hpp"
#include "button.hpp"
struct Hint {
    bool hintLoaded = false;
    bool flagtoLoad = false;
    std::string data;

    void getHint(const std::string & word);
    void drawHintBtn(void);
    void reset(void);
};