#include "textBox.hpp"
#include <iostream>
void drawTextBox(const Rectangle & rec, float thickness,Color color, int fontSize, const std::string & text) {
std::string renderedText {text};
if(text.size() > 14) {
    renderedText = text.substr(text.length() - 14, 14);
}
int size = MeasureText(renderedText.c_str(), fontSize);
int x = rec.x + 5;
int y = rec.y + (rec.height - fontSize) / 2;
DrawRectangleLinesEx(rec, thickness, color);
DrawText(renderedText.c_str(),x, y, fontSize, LIGHTGRAY);
}
