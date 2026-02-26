#include "wordle.hpp"

int Wordly::centerTextByX(const std::string & text, int fontSize, int width, int marginX = 0) const {
int textWidth = MeasureText(text.c_str(), fontSize);

return marginX + (width - textWidth) / 2;
}


void drawPattern(void) {
    int square = 40;
    int spaces = 10;

    for(int i = 0; i < GetScreenHeight(); i += square + spaces) {
        for(int j = 0; j < GetScreenWidth(); j += square + spaces) {
            DrawRectangleLines(j, i, square, square, Fade(GRAY, 0.1f));
        }
    }
}

void Wordly::drawFrontScreen(void) {
    ClearBackground({18, 19, 19, 255});
    drawLogo();
    drawPattern();
    const int numButtons = 4;
    static const std::vector<std::string> buttons = {"Daily challenge", "Pratice mode", "Autoplay showcase", "Leaderboard", "PVP", "Exit"};
    float btnW = 280.f;
    float btnH  = 40.f;
    float startX = (GetScreenWidth() - (float) btnW) / 2;
    float startY = 200;
    float spacing = 15;
    for(int i = 0; i < buttons.size(); i++) {

        Rectangle rec = {startX, startY + i * (btnH + spacing), btnW, btnH};
        bool isHovered = (CheckCollisionPointRec(GetMousePosition(), rec));
        Color color = isHovered ? Color{106,170,100,255} : Color{83, 141, 78, 255};
        if(!dailyChallenge.first && i == 0) {
            color = Fade(WHITE, 0.05f);
        }
        if(isHovered) {
            rec.height += 3;
            rec.x += 4;
        }
        Button btn;
        btn = btn.drawBtn(rec, buttons[i], color);

        if(btn.checkClick(GetMousePosition())) {

            switch (i) {
            case 0 :    
            if(dailyChallenge.first) {
                state = DAILY_CHALLENGE; 
                mainTimer.start();
                getRandomWordDayChallenge();
                dailyChallenge.second = generateDayId();
            }
                break;
            case 1 :
                state = PRACTICE;
                mainTimer.start();
                getRandomWord();
                std::cout << word << std::endl;

                break;
            case 2 :
            state = AUTOPLAY;
             mainTimer.start();
            getRandomWord();
            this->config.autoplay = true;
            break;

            case 3:

            state = LEADERBOARD;
            break;

            case 4:
            state = PVP;
            manager.connect("ws://localhost:8000");
            break;

            case 5: 
            std::exit(0);
        }
    }
}
}

void Wordly::drawError(const std::string & msg) const {
    int fontSize = 20;
        int textSize = MeasureText(msg.c_str(),fontSize);
        float x = (GetScreenWidth() - textSize) / 2;
        DrawText(msg.c_str(), (int) x, 680, fontSize, RED);
    }
void Wordly::drawTimer(void) const {
        std::string text = mainTimer.getCurrentTime();
        int fontSize = 20;
        DrawText(text.c_str(), 20, 20, fontSize, LIGHTGRAY);
    }


void Wordly::renderKeyBoard(void) {
int posY = pos.y + SQUARE_SIZE + 30;
int posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
int CELL_SIZE = SQUARE_SIZE / 1.8;
std::string character;
for(auto & x : keyboard) {
    character.clear();
    character += x.c;
    Color color = x.status == NOT_CHECKED ? LIGHTGRAY : x.status == CORRECT ? GREEN : x.status == INCORRECT ? YELLOW : DARKGRAY;
    Rectangle box = {(float) posX, (float) posY, (x.c == "ENT" || x.c == "DEL") ? (float) (CELL_SIZE * 1.5)
         : (float) CELL_SIZE, (float) CELL_SIZE};
    Vector2 textSize = MeasureTextEx(GetFontDefault(), character.c_str(), 18.f, 2);
    float textX = box.x + (box.width / 2) - (textSize.x / 2);
    float textY = box.y + (box.height / 2) - (textSize.y / 2);
    bool specialButton  = x.c == "ENT" || x.c == "DEL" ? 1 : 0;
    int size = 0;
    if(specialButton) {
         x.box = Rectangle(posX, posY, CELL_SIZE * 1.5, CELL_SIZE);
         size = CELL_SIZE * 1.5;
    }
    else {
        size = CELL_SIZE;
         x.box = Rectangle(posX, posY, size,size);
    }
   
    DrawRectangle(posX, posY, size, CELL_SIZE, color);
    DrawText(character.c_str(), (int) textX, (int) textY, 18, BLACK);

    posX += size + 6;

    if(x.c == "p" || x.c == "l") {
        posY += CELL_SIZE + 6;
        posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
    }

}
}

void Wordly::drawUsername(void) const {
    int fontSize = 25;
    int paddingX = 15;
    
    int width = MeasureText(this->username.c_str(), fontSize);
    int x = GetScreenWidth() - width - paddingX;
    int y = paddingX;
    Rectangle rec = {(float) x - 10, (float) y - 5, (float) width + 20, (float) fontSize + 10};
    DrawRectangleRounded(rec, 0.5f, 10, ColorAlpha(BLACK, 0.3f));
    DrawText(this->username.c_str(), x, y, fontSize, RAYWHITE);
}


void Wordly::drawGrid(const float offset) {
    int marginX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
       for(size_t i = 0; i < 6; i++) {
        float currentRowOffset = i == activeY ? offset : 0.0f;
        for(size_t j = 0; j < 5; j++) {
            auto c = this->history[i][j];
        float calculateX = (float) ((j * SQUARE_SIZE * 1.1) + marginX + 15) + currentRowOffset;
        float calculateY =  (float) ((i * SQUARE_SIZE * 1.1) + 90);
        if(i == 5) {
            pos.y = (i * SQUARE_SIZE * 1.1) + 90;
        }
        Rectangle box = {(float) calculateX,calculateY, SQUARE_SIZE, SQUARE_SIZE};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), std::string{c.c}.c_str(), FONT_SIZE, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(std::string{c.c}.c_str(), (int) textX, (int) textY, FONT_SIZE, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, this->config.grid_color);
       }
       }
}

void Wordly::drawOriginalStateGame(void) {
mainTimer.update();
    drawTimer();
    drawLogo();
        drawUsername();
    if(!gameOver) {
    float offset = 0.0f;
    if(pendingGameOver) {
        mainTimer.stop();
        timer -= GetFrameTime();

        if(timer <= 0) {
            pendingGameOver = false;
            gameOver = true;
            timer  =0;
        }
    }
    else {
        if(this->config.autoplay) {
            autoBotPlay();
        }
    }
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


void Wordly::drawPvp(void) {
    drawLogo();
    drawUsername();
      if(manager.getStatus()) {
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
          Button btn;
    Rectangle rec = {(float) GetScreenWidth() / 2 - 60, (float) GetScreenHeight() - 80, 120, 40};
     Color colorBtn = DARKGRAY;
    if(CheckCollisionPointRec(GetMousePosition(), rec)) {
        colorBtn = LIGHTGRAY;
    }
    btn = btn.drawBtn(rec, "Back", colorBtn);
    if(btn.checkClick(GetMousePosition())) {
        manager.disconnect();
         clearVariables();
         manager.isWaitingForServer = false;
         manager.packet.received = false;
        state = MAIN_MENU;
    }
    }
  
}


void Wordly::drawPvpWin(void) const {
    const char * text = "VICTORY!";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = fontSize;
    DrawText(text, posX + 4, posY + 4, fontSize, MAROON);
    DrawText(text, posX, posY, fontSize, GOLD);
}

void Wordly::drawPvpLose(void) const {
    const char * text = "You lost";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY = fontSize;
    DrawText(text, posX + 4, posY + 4, fontSize, BLUE);
    DrawText(text, posX, posY, fontSize, RED);
}

void Wordly::drawPvpDraw(void) const {
    const char * text = "DRAW";
    int fontSize = 60;

    int textW = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textW) / 2;
    int posY  = fontSize;
    DrawText(text, posX + 4, posY + 4, fontSize, MAROON);
    DrawText(text, posX, posY, fontSize, LIGHTGRAY);
}