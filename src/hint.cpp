#include "hint.hpp"
#include "utils.hpp"
void Hint::getHint(const std::string & word) {
    std::cout << "here" << std::endl;
        ParserJSON parser;
        parser.insert("word", word);
        std::string data = parser.toString();
        std::string buffer = postRequest("http://localhost:3000/get-hint", data);
        if(!utils::isEmpty(buffer)) {
            this->data = buffer;
            flagtoLoad = false;
            hintLoaded = true;
        }
}

void Hint::drawHintBtn(void) {
    int y = GetScreenHeight() - 40;
    int x = 50;
    int height = 30;
    int width = 130;

    Rectangle rec = {(float) x, (float) y, (float)width, (float)height};
    Color color = CheckCollisionPointRec(GetMousePosition(), rec) ? DARKGRAY : RAYWHITE;
    Button btn (rec, color, "Hint");
    btn.drawBtn();
    if(btn.checkClick(GetMousePosition())) {
        flagtoLoad = true;
    }
}

void Hint::reset(void) {
    data.clear();
    flagtoLoad = false;
    hintLoaded = false;
}