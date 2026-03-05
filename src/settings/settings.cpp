#include "settings.hpp"

void Settings::drawSettings(void) {
    Color modalBg = {39, 39, 42, 255};
    Color borderColor = {63, 63, 70, 255};
    Color accentColor = {34, 197, 94, 255};
    const float width = 350.0f;
    const float height = 450.0f;
    Rectangle rec = {(GetScreenWidth() - width) / 2.0f, (GetScreenHeight() - height) / 2.0f, width, height};

    DrawRectangleRounded({rec.x + 5, rec.y + 5, rec.width, rec.height}, 0.1f, 10, ColorAlpha(BLACK, 0.3f));

    DrawRectangleRounded(rec, 0.1f, 10, modalBg);
    DrawRectangleRoundedLines(rec, 0.1f, 10,borderColor);

    DrawText("SETTINGS", (int) rec.x + 20, (int) rec.y + 20, 25, RAYWHITE);

    DrawLine(rec.x + 20, rec.y + 55, rec.x + width - 20, rec.y + 55, borderColor);

    float startY = rec.y + 80;

    DrawText("Hard Mode", rec.x + 30, startY, 20, RAYWHITE);

    Rectangle checkHardModeRec = {rec.x + rec.width - 60, startY, 25, 25};

    if(CheckCollisionPointRec(GetMousePosition(), checkHardModeRec)) {
        DrawRectangleLinesEx(checkHardModeRec, 2, YELLOW);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            hardMode = !hardMode;
        }

    } else {
        DrawRectangleLinesEx(checkHardModeRec, 2, GRAY);
    }

    if(hardMode) {
        DrawRectangle(checkHardModeRec.x + 5, checkHardModeRec.y + 5, 15, 15, GREEN);
    }
}