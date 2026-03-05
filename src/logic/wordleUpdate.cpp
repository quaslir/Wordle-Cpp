#include "../wordle.hpp"

void Wordly::update(void) {
if(state == EMPTY_USERNAME) {
        drawLogo();
        setUsername();
        return;
    }
    if(!gameOver) {
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
        if(state == AUTOPLAY) {
            autoBotPlay();
        }
    }
    if(state == DAILY_CHALLENGE || state == PRACTICE) {
    writeKey();
    }
    
} else {
        gameOverScreenRenderer();
    }
}

void Wordly::updatePvp(void) {

    manager.receive();

    if(manager.packet.received) {

        if(manager.isWaitingForServer) {
        manager.isWaitingForServer = false;
        if(manager.packet.error) {
            renderErrorMessage = true;
                    errorMessage = "Incorrect word";
                    shakeTimer = 0.5f;
        }
        else {
             updateKeyStatus();
        }
        }
    
        else if(!manager.wordReceved){
        this->word = manager.packet.word;
        manager.wordReceved = true;
    }
        

        manager.packet.received = false;
               
    }
}

void Wordly::play(void) {

update();
if((state == DAILY_CHALLENGE || state == PRACTICE || state == AUTOPLAY) && !openSettings) {
    readKey();
    drawOriginalStateGame();
}
else if(state == LEADERBOARD) {
    if(!leaderboard.leaderboardLoaded) {
        try {
    leaderboard.loadLeaderboard();
        } catch(const std::exception & error) {
            std::cerr << error.what() << std::endl;
            state = MAIN_MENU;
        }
    } else leaderboard.renderLeaderboard();
    
}

else if(state == PVP) { 
    drawPvp();

        
    updatePvp();
        if(manager.packet.turn) {
            if(!renderErrorMessage) {
            errorMessage = "Your turn";
            }
            readKey();
            writeKey();
    } else errorMessage.clear();
    if(!manager.getStatus() && !manager.gameOver) {
        DrawText("WAITING FOR OPPONENT...", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2, 20, DARKGRAY);
    }
    

        if(manager.gameOver) {
                size_t xp = calculateXpDistribution();
                if(!leaderboard.leaderboardUpdated) {
                    if(manager.packet.win) {
                        std::thread([&] {
                            leaderboard.updateLeaderboard(username, totalXp + xp);
                        }).detach();
                    }
                    else if(!manager.packet.win && !manager.packet.draw) {
                   std::thread([&] {
                            leaderboard.updateLeaderboard(username, totalXp - xp);
                        }).detach();
                    }
                    leaderboard.leaderboardUpdated = true;
                    leaderboard.receiveUsersXp(this->username);
                }
        if(manager.packet.win) {

            drawPvpWin();
        }
        else if(!manager.packet.win && !manager.packet.draw) {
            drawPvpLose();
        }
        else drawPvpDraw();
        drawXp(xp);
    } 
}

else if(state == EMPTY_USERNAME) {
 drawUsername();
} else {   
     drawFrontScreen();
    
}


if(openSettings) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.5f));

    settings.drawSettings();
}
}

void Wordly::updateDailyChallengeStatus(void) {
    try {
        auto dailyCh = usersHistory.getObject("daily_challenge").value();
    dailyCh.updateValue<std::string>("daily_challenge_active", "false");
    dailyCh.updateValue<std::string>("daily_challenge_id", std::to_string(generateDayId()));
    usersHistory.updateValue<std::string>("daily_challenge", dailyCh.toString());
    dailyChallenge.first = false;
    } catch(...) {
        std::cerr << "Json file was corrupted" << std::endl;
    } 
}


void Wordly::updateKeyStatus(void) {

    size_t idx = 0;
        for(auto & c : history[activeY]) {
            if(this->word.find(c.c) == std::string::npos) {
                c.type = NOT_IN;
                auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = INVALID;
                }
            }
            else if(this->word[idx] == c.c) {
                 c.type  = CORRECT_POS;
                 if(settings.hardMode) {
                    this->mustUsedChars.insert(c.c);
                 }
                 auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = CORRECT;
                }
            }
            else  {
                c.type = INCORRECT_POS;
                 if(settings.hardMode) {
                    this->mustUsedChars.insert(c.c);
                 }
                 auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = INCORRECT;
                }
            }
            idx++;
        }
                activeX = 0;
        if(activeY < 6) {
             activeY++;
        }  
        attempts++;

}