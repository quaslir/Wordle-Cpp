#include "../wordle.hpp"
#include <exception>
void Wordly::update(void) {
    if(openSettings) return;
if(gameState.state == EMPTY_USERNAME) {
        drawLogo();
        setUsername();
        return;
    }
    if(!gameState.gameOver) {
    if(pendingGameOver) {
        gameState.mainTimer.stop();
        timer -= GetFrameTime();

        if(timer <= 0) {
            pendingGameOver = false;
            gameState.gameOver = true;
            timer  =0;
        }
    }
    else {
        if(gameState.state == AUTOPLAY) {
            autoBotPlay();
        }
    }
    if(gameState.state == DAILY_CHALLENGE || gameState.state == PRACTICE) {
    readKey();
    writeKey();
    }
    
    }
}

void Wordly::updatePvp(void) {

    manager.receive();

    if(manager.packet.received) {

        if(manager.isWaitingForServer) {
        manager.isWaitingForServer = false;
        if(manager.packet.error) {
            view.renderErrorMessage = true;
                    view.errorMessage = "Incorrect word";
                    view.shakeTimer = 0.5f;
        }
        else {
             updateKeyStatus();
        }
        }
    
        else if(!manager.wordReceved){
        gameState.word = manager.packet.word;
        manager.wordReceved = true;
    }
        

        manager.packet.received = false;
               
    }
}

void Wordly::play(void) {
gameState.mainTimer.update();
update();
if((gameState.state == DAILY_CHALLENGE || gameState.state == PRACTICE || gameState.state == AUTOPLAY) && !openSettings) {
    
    gameState.mainTimer.drawTimer();
    view.drawOriginalStateGame();
}
else if(gameState.state == LEADERBOARD) {
    if(!leaderboard.leaderboardLoaded) {
        try {

        leaderboard.loadLeaderboard();

        } catch(const std::exception & error) {
            std::cerr << error.what() << std::endl;
            gameState.state = MAIN_MENU;
        }
    } else leaderboard.renderLeaderboard();
    
}

else if(gameState.state == PVP) { 
        pos.y = (5 * SQUARE_SIZE * 1.1) + 90;
    view.drawPvp();
    if(manager.gameOver && !manager.connected()) {
        clearVariables();
        manager.disconnect();
        gameState.state = MAIN_MENU;
        manager.isWaitingForServer = false;
         manager.packet.received = false;
         manager.packet.win = false;
         manager.packet.draw = false;
         manager.gameOver = false;
         leaderboard.leaderboardUpdated = false;
        return;
    }
        
    updatePvp();
        if(manager.packet.turn) {
            if(!view.renderErrorMessage) {
            view.errorMessage = "Your turn";
            }
            readKey();
            writeKey();
    } else view.errorMessage.clear();
    if(!manager.getStatus() && !manager.gameOver) {
        DrawText("WAITING FOR OPPONENT...", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2, 20, DARKGRAY);
    }
    

        if(manager.gameOver) {
                size_t xp = calculateXpDistribution();
                if(!leaderboard.leaderboardUpdated) {
                    if(manager.packet.win) {
                        std::thread([&] {
                            leaderboard.updateLeaderboard(user.username, user.totalXp + xp);
                        }).detach();
                    }
                    else if(!manager.packet.win && !manager.packet.draw) {
                   std::thread([&] {
                            leaderboard.updateLeaderboard(user.username, user.totalXp - xp);
                        }).detach();
                    }
                    leaderboard.leaderboardUpdated = true;
                    leaderboard.receiveUsersXp(user.username);
                }
        if(manager.packet.win) {

            view.drawPvpWin();
        }
        else if(!manager.packet.win && !manager.packet.draw) {
            view.drawPvpLose();
        }
        else view.drawPvpDraw();
        view.drawXp(xp, manager.packet.win, manager.packet.draw);
    } 
}

else if(gameState.state == EMPTY_USERNAME) {
 view.drawUsername();
} else {   
     view.drawFrontScreen();
    
}


if(openSettings) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.5f));

    settings.drawSettings();
}
}

void Wordly::updateDailyChallengeStatus(void) {
    try {
        auto dailyCh = user.usersHistory.getObject("daily_challenge").value();
    dailyCh.updateValue<std::string>("daily_challenge_active", "false");
    dailyCh.updateValue<std::string>("daily_challenge_id", std::to_string(generateDayId()));
    user.usersHistory.updateValue<std::string>("daily_challenge", dailyCh.toString());
    user.dailyChallenge.first = false;
    } catch(const std::exception & error) {
        std::cerr << error.what() << std::endl;
    } 
}


void Wordly::updateKeyStatus(void) {

        size_t idx = 0;
        for(auto & c : gameState.history[view.activeY]) {
            if(gameState.word.find(c.c) == std::string::npos) {
                c.type = NOT_IN;
                auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = INVALID;
                }
            }
            else if(gameState.word[idx] == c.c) {
                 c.type  = CORRECT_POS;
                 if(settings.hardMode) {
                    gameState.mustUsedChars.insert(c.c);
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
                    gameState.mustUsedChars.insert(c.c);
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
                view.activeX = 0;
        if(view.activeY < 6) {
             view.activeY++;
        }  
        gameState.attempts++;

}