#include "wordle.hpp"
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
    gameState.mainTimer.update();
    readKey();
    writeKey();
    }
    
    }
}

void Wordly::handleLeaderboardState(void) {
    try {
        leaderboard.loadLeaderboard();

        } catch(const std::exception & error) {
            std::cerr << error.what() << std::endl;
            gameState.state = MAIN_MENU;
        }
}


void Wordly::handlePvpState(void) {
pos.y = (5 * SQUARE_SIZE * 1.1) + 90;
    view.drawPvp();
    if(pvp.gameOver && !pvp.connected()) {
        clearVariables();
        pvp.disconnect();
        gameState.state = MAIN_MENU;
         leaderboard.leaderboardUpdated = false;
        return;
    }
        
    updatePvp();
        if(pvp.packet.turn) {
            if(!view.renderErrorMessage) {
            view.errorMessage = "Your turn";
            }
            readKey();
            writeKey();
    } else view.errorMessage.clear();
    if(!pvp.getStatus() && !pvp.gameOver) {
        DrawText("WAITING FOR OPPONENT...", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2, 20, DARKGRAY);
    }
    

        if(pvp.gameOver) {
                size_t xp = calculateXpDistribution();
                if(!leaderboard.leaderboardUpdated) {
                    if(pvp.packet.win) {
                        std::thread([&] {
                            leaderboard.updateLeaderboard(user.username, user.totalXp + xp);
                        }).detach();
                    }
                    else if(!pvp.packet.win && !pvp.packet.draw) {
                        size_t newXp = user.totalXp - xp < 0 ? 0 : user.totalXp - xp;
                   std::thread([&] {
                            leaderboard.updateLeaderboard(user.username, newXp);
                        }).detach();
                    }
                    leaderboard.leaderboardUpdated = true;
                    leaderboard.receiveUsersXp(user.username);
                }
        if(pvp.packet.win) {

            view.drawPvpWin();
        }
        else if(!pvp.packet.win && !pvp.packet.draw) {
            view.drawPvpLose();
        }
        else view.drawPvpDraw();
        view.drawXp(xp, pvp.packet.win, pvp.packet.draw);
    }
}


void Wordly::updatePvp(void) {

    pvp.receive();

    if(pvp.packet.received) {

        if(pvp.isWaitingForServer) {
        pvp.isWaitingForServer = false;
        if(pvp.packet.error) {
                    view.setErrorMsg("Incorrect word");
        }
        else {
             updateKeyStatus();
        }
        }
    
        else if(!pvp.wordReceved){
        gameState.word = pvp.packet.word;
        pvp.wordReceved = true;
    }
        

        pvp.packet.received = false;
               
    }
}

void Wordly::play(void) {

update();
if((gameState.state == DAILY_CHALLENGE || gameState.state == PRACTICE || gameState.state == AUTOPLAY) && !openSettings) {
    
    gameState.mainTimer.drawTimer();
    view.drawOriginalStateGame();
}
else if(gameState.state == LEADERBOARD) {
    if(!leaderboard.leaderboardLoaded) {
        handleLeaderboardState();
    } else leaderboard.renderLeaderboard();
    
}

else if(gameState.state == PVP) { 
         handlePvpState();
}

else if(gameState.state == EMPTY_USERNAME) {
 view.drawUsername();
} else {   
     view.drawFrontScreen();
    
}

if(openSettings) {
    settings.drawSettings();
}
}

void Profile::updateDailyChallengeStatus(void) {
    try {
        auto dailyCh = usersHistory.getObject("daily_challenge").value();
    dailyCh.updateValue<std::string>("daily_challenge_active", "false");
    dailyCh.updateValue<std::string>("daily_challenge_id", std::to_string(utils::generateDayId()));
    usersHistory.updateValue<std::string>("daily_challenge", dailyCh.toString());
    dailyChallenge.first = false;
    } catch(const std::exception & error) {
        std::cerr << error.what() << std::endl;
    } 
}


void Wordly::updateKeyStatus(void) {

        size_t idx = 0;
        for(auto & c : gameState.history[view.activeY]) {
            if(gameState.word.find(c.c) == std::string::npos) {
                c.type = NOT_IN;
                keyboard.updateKeyboard(c.c, INVALID);
            }
            else if(gameState.word[idx] == c.c) {
                 c.type  = CORRECT_POS;
                 if(settings.hardMode) {
                    gameState.mustUsedChars.insert(c.c);
                 }
                
                keyboard.updateKeyboard(c.c, CORRECT);
            }
            else  {
                c.type = INCORRECT_POS;
                 if(settings.hardMode) {
                    gameState.mustUsedChars.insert(c.c);
                 }
                 keyboard.updateKeyboard(c.c, INCORRECT);
            }
            idx++;
        }
                view.activeX = 0;
        if(view.activeY < 5) {
             view.activeY++;
        }  
        gameState.attempts++;

}