#include "view.hpp"
#include "button.hpp"
void ViewContext::gameOverScreenRenderer(void)const {

    
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.5f));
    Rectangle panel = {40, 100, (float) GetScreenWidth() - 80, (float) GetScreenHeight() - 120};

    DrawRectangleRec(panel, ColorAlpha(DARKGRAY, 0.9f));
    DrawRectangleLinesEx(panel,2 , GREEN);

    float titleSize = GetScreenHeight() * 0.055f;
    float subTitleSize = GetScreenHeight() * 0.03f;
    float textStartY = panel.y + titleSize + 40;
    if(getUserWon()) {

    DrawText("WELL DONE!", panel.x + 20, panel.y + 20, titleSize, GREEN); 
    std::string text;
    int minutes = (int) getMins();
    int seconds = (int) getSeconds();
    if(minutes > 0) {
        text = std::to_string(minutes) + (minutes == 1 ? " minute " : "minutes ");
        if(seconds > 0) text += "and " + std::to_string(seconds) + " seconds";
    }  
    else {
        text += std::to_string(seconds) + " seconds";
    }

    const char *wordLine = TextFormat("You guessed: \"%s\"", getWord().c_str());
    const char *timeLine = TextFormat("Time: %s", text.c_str());

    DrawText(wordLine, panel.x + 20, textStartY, subTitleSize, WHITE);
    DrawText(timeLine, panel.x + 20, textStartY + subTitleSize + 10, subTitleSize, LIGHTGRAY);
    } else {
        DrawText("NEXT TIME...", panel.x + 20, panel.y + 20, titleSize, RED);
        DrawText("THE WORD WAS: ", panel.x + 20, textStartY, subTitleSize, LIGHTGRAY);
        std::string word = "\"" + getWord() + "\"";

        float labelSize = MeasureText("THE WORD WAS: ", subTitleSize);

        DrawText(word.c_str(), panel.x + 20 + labelSize, textStartY, subTitleSize, WHITE);

        DrawText("Don't give up!", panel.x + 20, textStartY + subTitleSize + 10, subTitleSize * 0.8f, GRAY);  
    }
    

    float startY = textStartY + (2 * (subTitleSize + 20));
        float sectionSize = GetScreenHeight() * 0.04f;
    auto drawStatRow = [&](const std::string & label, const std::string & key, int x) {
        auto val = getValue(key);
        if(val.has_value()) {
            DrawText(std::to_string(val.value()).c_str(), x, startY, sectionSize, WHITE);
            DrawText(label.c_str(), x, startY + sectionSize + 5 , sectionSize / 1.8, LIGHTGRAY);
        }
    };


    std::vector<std::pair<std::string, std::string>> stats = {{"Total games", "total_games"}, 
{"Wins", "wins"}, {"Losses", "losses"}, {"Current streak", "current_streak"}, {"Best streak", "best_streak"}};
    float currentX = panel.x + 30;
    for(const auto & stat: stats) {
        drawStatRow(stat.first, stat.second, currentX);

        currentX += MeasureText(stat.first.c_str(), sectionSize / 1.5);
    }
        drawGuessDistribution(panel, startY + sectionSize * 3);

    float width = panel.width / 4.0f;
    float height = panel.height * 0.035f;
    float btnX = panel.x + (panel.width - (width * 2 + 10.0f)) / 2.0f;
    float btnY = panel.y + panel.height - 200;
    Rectangle box = {btnX, btnY, width, height};
    std::string text = "Play again";
    Color btnPlayAgainColor = CheckCollisionPointRec(GetMousePosition(), box) ? DARKGREEN : GREEN;
    Button playAgain (box, btnPlayAgainColor, text);
    playAgain.drawBtn();
        if(playAgain.checkClick(GetMousePosition())) {
            onPlayAgain();
    }

    Rectangle box2 =  {btnX + width + 10, btnY, width, height};
    Color btnExitColor = CheckCollisionPointRec(GetMousePosition(), box2) ? DARKGREEN : GREEN;
    std::string text2 = "Exit";
    Button exit (box2, btnExitColor, text2);
    exit.drawBtn();

    if(exit.checkClick(GetMousePosition())) {
        onExit();
    }
}

    void ViewContext::drawGuessDistribution(const Rectangle & rec, float startY) const {
        int maxWidth = GetScreenWidth();
        int maxWins = 0;
        auto distribution = getDistribution();
        if(distribution.has_value()) {
            for(int i = 1; i <= 6; i++) {
                auto current = distribution->getValue<int>(std::to_string(i));
                if(current.has_value()) {
                    if(current.value() > maxWins) maxWins = current.value();
                }
            }
                    float fontSize = GetScreenHeight() * 0.025f;
                    float rectangleH = GetScreenHeight() * 0.035f;
                    float spacing = GetScreenHeight() * 0.01f;
            for(int i = 1; i <= 6; i++) {
                float currentY = startY + (i-1) * (spacing + rectangleH);
                 auto current = distribution->getValue<int>(std::to_string(i));
                if(current.has_value()) {
                    int val = current.value();
                    float barW = 30;
                    if(current.value() != 0) {
                      barW = 25.0f + ((float) val /  maxWins) * (rec.width - 100.0f);
                    }
                    Color barC = (getAttempts() == i) ? GREEN : DARKGRAY;

                    DrawText(std::to_string(i).c_str(), rec.x + 20, currentY + (rectangleH / 4),fontSize,WHITE);
                    DrawRectangle((rec.x + 20) + fontSize, currentY, (int) barW, (int) rectangleH, barC);

                    std::string valStr = std::to_string(val);
                    int textWidth = MeasureText(valStr.c_str(), (int) (fontSize * 2/3));
                    DrawText(valStr.c_str(), rec.x + 35 + (int) barW - textWidth,
                currentY + (rectangleH - fontSize * 2 / 3) / 2, fontSize * 2/3, WHITE);

                }
            }
        }

    }