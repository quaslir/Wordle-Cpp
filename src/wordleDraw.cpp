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
    float btnW = GetScreenWidth() * 0.5f;
    float btnH  = GetScreenHeight() * 0.07f;
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
        float fontSize = GetScreenHeight() * 0.025f;
        int textSize = MeasureText(msg.c_str(), (int) fontSize);
        float x = (GetScreenWidth() - textSize) / 2;
        float y = (GetScreenHeight() - 60);
        DrawText(msg.c_str(), (int) x, (int) y, (int) fontSize, RED);
    }
void ViewContext::renderKeyBoard(void) const {
float startY = GetScreenHeight() * 0.7f;
float availableWidth = GetScreenWidth() * 0.9f;

float keyWidth = availableWidth / 11.0f;
float keyHeight = (GetScreenHeight() - startY) / 5.0f;

float CELL_SIZE = (keyWidth < keyHeight) ? keyWidth : keyHeight;
float spacing = CELL_SIZE * 0.15f;
float posY = startY;
float posX = (GetScreenWidth() - (10 * (CELL_SIZE + spacing))) / 2.0f;

std::string character;
auto & keys = const_cast<std::vector<Key>&>(getKeyboard());

for(auto & x : keys) {

    character.clear();
    character += x.c;
    Color color = x.status == NOT_CHECKED ? LIGHTGRAY : x.status == CORRECT ? GREEN : x.status == INCORRECT ? YELLOW : DARKGRAY;
    Rectangle box = {(float) posX, (float) posY, (x.c == "ENT" || x.c == "DEL") ? (float) (CELL_SIZE * 1.5)
         : (float) CELL_SIZE, (float) CELL_SIZE};

        
         bool onHover = CheckCollisionPointRec(GetMousePosition(), box) ? 1 : 0;
         if(onHover) {
            DrawRectangleLinesEx({(float) posX, (float) posY, box.width, box.height}, 2, Fade(WHITE, 0.7f));
        }

    Vector2 textSize = MeasureTextEx(GetFontDefault(), character.c_str(), 18.f, 2);
    float textX = box.x + (box.width / 2) - (textSize.x / 2);
    float textY = box.y + (box.height / 2) - (textSize.y / 2);
    bool specialButton  = x.c == "ENT" || x.c == "DEL" ? 1 : 0;
    int size = CELL_SIZE;
        DrawRectangle(posX, posY, box.width, box.height, color);
       
        setKey(box, x);
   if(specialButton) {
    posX += (size * 1.5) + spacing;
   }
   else {
    posX += size + spacing;
     
   }
   
    DrawText(character.c_str(), (int) textX, (int) textY, 18, BLACK);
   
    if(x.c == "p" || x.c == "l") {
        posY += CELL_SIZE + spacing;
        posX = (GetScreenWidth() - (10 * (CELL_SIZE + spacing))) / 2.0f;
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
    float gridheight = GetScreenHeight() * 0.6f;
    float gridwidth = static_cast<float>(GetScreenWidth());

    float adaptiveSize = optimizedSize();

    float spacing = adaptiveSize * 0.1f;

    float totalGridWidth = (adaptiveSize * 5) + (spacing * 4);
    float totalGridHeight = (adaptiveSize * 6) + (spacing * 5);

        float marginX = (gridwidth - totalGridWidth) / 2.0f;
        float marginY = (gridheight - totalGridHeight) / 2.0f + adaptiveSize * 1.1f;

        float adaptiveFontSize = adaptiveSize * 0.5f;
       for(size_t i = 0; i < 6; i++) {
        float currentRowOffset = i == activeY ? offset : 0.0f;
        for(size_t j = 0; j < 5; j++) {
            Character c = getChar(j, i);

        float calculateX = marginX + (j * (adaptiveSize + spacing)) + currentRowOffset;
        float calculateY = marginY + (i * (adaptiveSize + spacing));

        Rectangle box = {calculateX,calculateY, adaptiveSize, adaptiveSize};
        DrawRectangleLinesEx(box, 3.0f, LIGHTGRAY);
            Vector2 textSize = MeasureTextEx(GetFontDefault(), std::string{c.c}.c_str(), FONT_SIZE, 2);
            float textX = box.x + (box.width / 2.0f) - (textSize.x / 2.0f);
            float textY = box.y + (box.height / 2.0f) - (textSize.y / 2.0f);
        DrawText(std::string{c.c}.c_str(), (int) textX, (int) textY, adaptiveFontSize, getColor(c.type));


        
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
    onHint();

     if(renderErrorMessage || !errorMessage.empty()) {
        drawError(errorMessage);
    }
    } else {
        gameOverScreenRenderer();
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
    int fontSize = (int) (GetScreenHeight() * 0.15f);

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
   int posY  = (int) (GetScreenHeight() * 0.16f);
    DrawText(text, posX + 4, posY + 4, fontSize, MAROON);
    DrawText(text, posX, posY, fontSize, GOLD);
}

void ViewContext::drawPvpLose(void) const {
    const char * text = "You lost";
    int fontSize = (int) (GetScreenHeight() * 0.15f);

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = (int) (GetScreenHeight() * 0.16f);
    DrawText(text, posX + 4, posY + 4, fontSize, BLUE);
    DrawText(text, posX, posY, fontSize, RED);
}

void ViewContext::drawPvpDraw(void) const {
    const char * text = "DRAW";
     int fontSize = (int) (GetScreenHeight() * 0.15f);

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = (int) (GetScreenHeight() * 0.16f);
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
    int posY = (int) ((GetScreenHeight() * 0.16f) + GetScreenHeight() * 0.15f + 20);
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

