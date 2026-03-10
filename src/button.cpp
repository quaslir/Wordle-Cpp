#include "button.hpp"

bool Button::checkClick(const Vector2 & pos)const{
        if(CheckCollisionPointRec(pos, btn)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return true;
            }
        }
        return false;
    }
bool Button::checkHover(const Vector2 & pos) const {
if(CheckCollisionPointRec(pos, btn)) {
    //SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    return true;
}
return false;
}

void Button::drawBtn(void) const{

    DrawRectangleRounded(btn, 0.2f, 10, color);
    const int fontSize = 20;
    int width = MeasureText(text.c_str(),fontSize);
    int x = btn.x + (btn.width - width) / 2.0f;
    int y = btn.y + (btn.height - fontSize) / 2.0f;
    DrawText(text.c_str(),x, y, 20, BLACK);
}