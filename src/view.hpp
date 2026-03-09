#pragma once
#include <string>
#include <functional>
#include "parser.hpp"
#include "raylib.h"
struct ViewContext {
int activeX = 0;
int activeY = 0;
float shakeTimer = 0.0f;
float shakeIntensity = 10.0f;
bool renderErrorMessage = false;
std::string errorMessage = "";
std::function<bool(void)> onDailyChallenge;
std::function<void(int i)> onClick;
std::function<std::string(void)> getUsername;
std::function<void(void)> onSettings;
std::function<void(void)> onPlayAgain;
std::function<void(void)> onExit;
std::function<size_t(void)> getAttempts;
std::function<bool(void)> getUserWon;
std::function<size_t(void)> getTotalXp;
std::function<std::string(void)> getWord;
std::function<float(void)> getMins;
std::function<float(void)> getSeconds;
std::function<const std::optional<ParserJSON> &(void)> getDistribution;
std::function<std::optional<int>(const std::string & key)> getValue;
void drawError(const std::string & msg) const;
void drawGuessDistribution(const Rectangle & rec) const;

void drawFrontScreen(void);
void drawUsername(void) const;
void drawPvpWin(void) const;
void drawPvpLose(void) const;
void drawPvpDraw(void) const;
void drawXp(int xp, bool win, bool draw) const;
void gameOverScreenRenderer(void);
};