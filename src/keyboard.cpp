#include "keyboard.hpp"
bool Key::clickStatus(void) const
{
    Button btn;
    btn.btn = box;
    return btn.checkClick(GetMousePosition());
}