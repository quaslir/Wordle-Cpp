#include "settings.hpp"
#include <array>
void Settings::drawSettings(void) {
    Color modalBg = {39, 39, 42, 255};
    Color borderColor = {63, 63, 70, 255};
    Color accentColor = {34, 197, 94, 255};
    const float width = GetScreenWidth() * 2.0f / 3.f;
    const float height = GetScreenHeight() * 0.7f;
    float fontSizeTitle = GetScreenHeight() * 0.03f;
    Rectangle rec = {(GetScreenWidth() - width) / 2.0f, (GetScreenHeight() - height) / 2.0f, width, height};

    DrawRectangleRounded({rec.x + 5, rec.y + 5, rec.width, rec.height}, 0.1f, 10, ColorAlpha(BLACK, 0.3f));

    DrawRectangleRounded(rec, 0.1f, 10, modalBg);
    DrawRectangleRoundedLines(rec, 0.1f, 10,borderColor);
    Rectangle exitSettingsRec = {rec.x + 20, rec.y + 20, 28, 28};
    Color colorExitSettingsBtn = CheckCollisionPointRec(GetMousePosition(), exitSettingsRec) ? DARKGRAY : LIGHTGRAY;
    Button exitSettingsBtn (exitSettingsRec, colorExitSettingsBtn, "<-");
    exitSettingsBtn.drawBtn();
    if(exitSettingsBtn.checkClick(GetMousePosition())) {
        onClose();
    }
    DrawText("SETTINGS", (int) rec.x + 65, (int) rec.y + 22, fontSizeTitle, RAYWHITE);

    DrawLine(rec.x + 20, rec.y + 55, rec.x + width - 20, rec.y + 55, borderColor);
       constexpr static std::array<std::string, 3> options = {"Hard Mode", "Offline mode", "Music"};
    for(int i = 0; i < options.size(); i++) {
    
    float startY = (rec.y + 80) + i * (50);
    DrawText(options[i].c_str(), rec.x + 30, startY, 20, RAYWHITE);

    Rectangle checkOptions = {rec.x + rec.width - 60, startY, 25, 25};
        if(i == 2) {
            checkOptions = Rectangle(rec.x + 30 ,startY + 27, rec.width - 90, 25);
            onSlider(checkOptions);
            continue;
        }
    if(CheckCollisionPointRec(GetMousePosition(), checkOptions)) {
        DrawRectangleLinesEx(checkOptions, 2, YELLOW);

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            switch(i) {
                case 0 :
                 hardMode = !hardMode;
                 break;
                 case 1:
                 offlineMode = !offlineMode;
                 break;
            }
           
        }

    } else {
        DrawRectangleLinesEx(checkOptions, 2, GRAY);
    }

    if(i == 0 && hardMode) {
        DrawRectangle(checkOptions.x + 5, checkOptions.y + 5, 15, 15, GREEN);
    }

    else if(i == 1 && offlineMode) {
        DrawRectangle(checkOptions.x + 5, checkOptions.y + 5, 15, 15, GREEN);
    }

}
    float btnWidth = 200.0f;
    float btnHeight = 40.0f;
    Rectangle btnBack = {
        rec.x + (rec.width - btnWidth) / 2.0f,
        rec.y + rec.height - 60.0f,
        btnWidth,
        btnHeight
    };


    bool isHoveredBtnBack = CheckCollisionPointRec(GetMousePosition(), btnBack);

    Color currentBtnBackColor = isHoveredBtnBack ? DARKGRAY : GRAY;
    std::string text = "MAIN MENU";
        Button backBtn (btnBack, currentBtnBackColor, text);
        backBtn.drawBtn();
    int fontSize = 20;

    float textX = btnBack.x + (btnBack.width - MeasureText(text.c_str(), fontSize)) / 2.0f;
    float textY = btnBack.y + (btnBack.height - fontSize) / 2.0f;

    DrawText(text.c_str(), (int)textX, (int) textY, fontSize, RAYWHITE);

    if(backBtn.checkClick(GetMousePosition())) {
        onState();
        onClose();
    }
}