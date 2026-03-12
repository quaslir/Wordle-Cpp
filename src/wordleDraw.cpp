#include "view.hpp"
#include "drawLogo.hpp"
#include "config.hpp"



void drawPattern(void) {
    int square = 40;
    int spaces = 10;

    for(int i = 0; i < GetScreenHeight(); i += square + spaces) {
        for(int j = 0; j < GetScreenWidth(); j += square + spaces) {
            DrawRectangleLines(j, i, square, square, Fade(GRAY, 0.1f));
        }
    }
}

void ViewContext::drawFrontScreen(void) const {
    ClearBackground({18, 19, 19, 255});
    drawLogo();
    drawPattern();
    drawUsername();
    if(getOpenSettings()) return;
    const int numButtons = 4;
    static const std::vector<std::string> buttons = {"Daily challenge", "Practice mode", "Autoplay showcase", "Leaderboard", "PVP", "Exit"};
    float btnW = 280.f;
    float btnH  = 40.f;
    float startX = (GetScreenWidth() - (float) btnW) / 2;
    float startY = 200;
    float spacing = 15;
    for(int i = 0; i < buttons.size(); i++) {

        Rectangle rec = {startX, startY + i * (btnH + spacing), btnW, btnH};
        bool isHovered = (CheckCollisionPointRec(GetMousePosition(), rec));
        Color color = isHovered ? Color{106,170,100,255} : Color{83, 141, 78, 255};
        if(!onDailyChallenge() && i == 0) {
            color = Fade(WHITE, 0.05f);
        }
        if(isHovered) {
            rec.height += 3;
            rec.x += 4;
        }
        Button btn (rec, color, buttons[i]);
        btn.drawBtn();

        if(btn.checkClick(GetMousePosition())) {
            onClick(i);
    }
}
}

void ViewContext::drawError(const std::string & msg) const {
        int fontSize = 20;
        int textSize = MeasureText(msg.c_str(),fontSize);
        float x = (GetScreenWidth() - textSize) / 2;
        DrawText(msg.c_str(), (int) x, 680, fontSize, RED);
    }
void ViewContext::renderKeyBoard(void) const {
Coordinates pos = getPos();

int posY = (5 * SQUARE_SIZE * 1.1 + 90) + SQUARE_SIZE + 30;
int posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
int CELL_SIZE = SQUARE_SIZE / 1.8;
std::string character;
auto & keys = const_cast<std::vector<Key>&>(getKeyboard());

for(auto & x : keys) {

    character.clear();
    character += x.c;
    Color color = x.status == NOT_CHECKED ? LIGHTGRAY : x.status == CORRECT ? GREEN : x.status == INCORRECT ? YELLOW : DARKGRAY;
    Rectangle box = {(float) posX, (float) posY, (x.c == "ENT" || x.c == "DEL") ? (float) (CELL_SIZE * 1.5)
         : (float) CELL_SIZE, (float) CELL_SIZE};
    Vector2 textSize = MeasureTextEx(GetFontDefault(), character.c_str(), 18.f, 2);
    float textX = box.x + (box.width / 2) - (textSize.x / 2);
    float textY = box.y + (box.height / 2) - (textSize.y / 2);
    bool specialButton  = x.c == "ENT" || x.c == "DEL" ? 1 : 0;
    int size = CELL_SIZE;
        DrawRectangle(posX, posY, box.width, box.height, color);
        setKey(box, x);
   if(specialButton) {
    posX += (size * 1.5) + 6;
   }
   else {
    posX += size + 6;
     
   }
   
    DrawText(character.c_str(), (int) textX, (int) textY, 18, BLACK);
   
    if(x.c == "p" || x.c == "l") {
        posY += CELL_SIZE + 6;
        posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
    }

}
}

void ViewContext::drawUsername(void) const{
    int fontSize = 25;
    int paddingX = 15;
    const float targetWidth = 50.0f;
    std::string username = getUsername();
    int width = MeasureText(username.c_str(), fontSize);
    int finalFontSize = fontSize;
    int finalWidth = width;
    if(targetWidth < width) {
        finalFontSize = static_cast<int>(fontSize * (targetWidth / width));
        finalWidth = MeasureText(username.c_str(), finalFontSize);
    }

    
    int x = GetScreenWidth() - finalWidth - paddingX;
    int y = (15 + (SQUARE_SIZE - finalFontSize)) / 2.0f;
    Rectangle rec = {(float) x - 10, (float) y - 5, (float) finalWidth + 20, (float) finalFontSize + 10};
    DrawRectangleRounded(rec, 0.5f, 10, ColorAlpha(BLACK, 0.3f));
    DrawText(username.c_str(), x, y, finalFontSize, RAYWHITE);

    if(CheckCollisionPointRec(GetMousePosition(), rec)) {
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        onSettings();
    }
} 
}


void ViewContext::drawGrid(const float offset) const{
    int marginX = (GetScreenWidth() - (SQUARE_SIZE * 5 * 1.1)) / 2;
       for(size_t i = 0; i < 6; i++) {
        float currentRowOffset = i == activeY ? offset : 0.0f;
        for(size_t j = 0; j < 5; j++) {
            Character c = getChar(j, i);
        float calculateX = (float) ((j * SQUARE_SIZE * 1.1) + marginX + 15) + currentRowOffset;
        float calculateY =  (float) ((i * SQUARE_SIZE * 1.1) + 90);

        Rectangle box = {(float) calculateX,calculateY, SQUARE_SIZE, SQUARE_SIZE};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), std::string{c.c}.c_str(), FONT_SIZE, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(std::string{c.c}.c_str(), (int) textX, (int) textY, FONT_SIZE, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, LIGHTGRAY);
       }
       }
}

void ViewContext::drawOriginalStateGame(void){
    
    drawLogo();
    drawUsername();
    if(!getGameOver()) {
    float offset = 0.0f;

    if(shakeTimer > 0) {
        shakeTimer -= GetFrameTime();
        offset = sinf(shakeTimer * 60.f) * shakeIntensity * (shakeTimer / 0.5f);
    }

    drawGrid(offset);
    
    renderKeyBoard();
    } else {
        gameOverScreenRenderer();
    }

    if(renderErrorMessage) {
        drawError(errorMessage);
    }
}


void ViewContext::drawPvp(void){
    drawLogo();
    drawUsername();
      if(getPvpStatus()) {
        float offset = 0.0f;
         if(shakeTimer > 0) {
        shakeTimer -= GetFrameTime();
        offset = sinf(shakeTimer * 60.f) * shakeIntensity * (shakeTimer / 0.5f);
    }
          drawGrid(offset);
          renderKeyBoard();
          if(!errorMessage.empty()) {
            drawError(errorMessage);
          }
    }
    else {
         
    Rectangle rec = {(float) GetScreenWidth() / 2 - 60, (float) GetScreenHeight() - 80, 120, 40};
     
     Color colorBtn = DARKGRAY;
 
    if(CheckCollisionPointRec(GetMousePosition(), rec)) {
        colorBtn = LIGHTGRAY;
    }
        Button btn (rec, colorBtn, "Back");
    btn.drawBtn();
    if(btn.checkClick(GetMousePosition())) {
        onBackBtn();
    }
    }
  
}


void ViewContext::drawPvpWin(void) const {
    const char * text = "VICTORY!";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = fontSize + 20;
    DrawText(text, posX + 4, posY + 4, fontSize, MAROON);
    DrawText(text, posX, posY, fontSize, GOLD);
}

void ViewContext::drawPvpLose(void) const {
    const char * text = "You lost";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY = fontSize + 20;
    DrawText(text, posX + 4, posY + 4, fontSize, BLUE);
    DrawText(text, posX, posY, fontSize, RED);
}

void ViewContext::drawPvpDraw(void) const {
    const char * text = "DRAW";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = fontSize + 20;
    DrawText(text, posX + 4, posY + 4, fontSize, MAROON);
    DrawText(text, posX, posY, fontSize, LIGHTGRAY);
}

void ViewContext::drawXp(int xp, bool win, bool draw) const {
    std::string score;
    if(win) {
       score +=  " (+" + std::string{std::to_string(xp)} + ")";
    }
    else if(!win && !draw) {
       score += " (-" + std::string{std::to_string(xp)} + ")";
    }

    std::string text = "Total xp: " + std::to_string((int) getTotalXp() + 
    (win ? xp : !win && !draw ? -xp : 0)) + score;
    int size = MeasureText(text.c_str(), 20);
    int posY = 150;
    int posX = (GetScreenWidth() - size) / 2;

    DrawText(text.c_str(), posX, posY, 20, RAYWHITE);

}   


    Color ViewContext::getColor(const Type & t)const {
        switch(t) {
            case INCORRECT_POS :
            return YELLOW;

            case CORRECT_POS :
            return GREEN;

            default: 
            return GRAY;
        }
        return GRAY;
    }

    void ViewContext::setErrorMsg(const std::string & msg) {
         errorMessage = msg;
         renderErrorMessage = true;
        shakeTimer = 0.5f;
    }