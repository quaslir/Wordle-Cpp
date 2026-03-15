#include "music.hpp"
#include <iostream>
#include "utils.hpp"
MusicManager::~MusicManager() {
    UnloadMusicStream(currentMusic);
    CloseAudioDevice();
}

void MusicManager::loadAllFromFolder(const std::string & path) {
    if(!std::filesystem::exists(path)) return;

    for(const auto & file : std::filesystem::directory_iterator(path)) {
        if(file.is_regular_file()) {
            std::string extenstion = file.path().extension().string();
            if(extenstion != ".mp3" && extenstion != ".wav" && extenstion != ".ogg") continue;
            std::string name = file.path().string();
            backgroundTracks.push_back(name);
        }
        else if(file.is_directory()) {
            for(const auto & f : std::filesystem::directory_iterator(file.path().string())) {
            std::string extenstion = f.path().extension().string();
            if(extenstion != ".mp3" && extenstion != ".wav" && extenstion != ".ogg") continue;
            std::string name = f.path().string();
            std::string key = name;
            utils::normalizeKey(key);
            eventTracks[key] = name;
            std::cout << key << std::endl;
            }
        }
    }
    currentIndex = backgroundTracks.size() - 1;
}

void MusicManager::setCurrentMusic(void) {

    if(currentMusic.ctxData != nullptr) {
        UnloadMusicStream(currentMusic);
        currentMusic = {0};
    }

    if(backgroundTracks.empty()) return;
    if(!activePlaylist) {
        currentMusic = LoadMusicStream(backgroundTracks[currentIndex--].c_str());
    }
    if(currentMusic.ctxData != nullptr) {
        PlayMusicStream(currentMusic);
        SetMusicVolume(currentMusic, volume);
    }

    if(currentIndex <= 0) currentIndex = backgroundTracks.size() - 1;
}

void MusicManager::updateMusic(void) {
if(currentMusic.ctxData == nullptr || !IsMusicValid(currentMusic)) return;
UpdateMusicStream(currentMusic);

float timePlayed = GetMusicTimePlayed(currentMusic);
float timeLength = GetMusicTimeLength(currentMusic);

if(timePlayed >= timeLength - 0.1f) {
    if(activePlaylist){
        activePlaylist = false;
        currentTrackPath.clear();
    }
    setCurrentMusic();
}
}

void MusicManager::playFromPlaylist(const std::string & key) {
if(currentTrackPath == eventTracks[key]) return;

currentTrackPath = eventTracks[key];

if(currentMusic.ctxData != nullptr) {
        UnloadMusicStream(currentMusic);
        currentMusic = {0};
}
currentMusic = LoadMusicStream(currentTrackPath.c_str());
activePlaylist = true;

if(currentMusic.ctxData != nullptr) {
        PlayMusicStream(currentMusic);
        SetMusicVolume(currentMusic, volume);
}

}

void MusicManager::drawSlider(const Rectangle & rec) {
    DrawRectangleRec(rec, LIGHTGRAY);

    float handleX = rec.x + (volume * rec.width);
    Vector2 handlePos = {handleX, rec.y + rec.height / 2};


    Vector2 mousePos = GetMousePosition();

    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if(CheckCollisionPointRec(mousePos, rec)) {
            float newVolume = (mousePos.x - rec.x) / rec.width;
            if(newVolume < 0.0f) newVolume = 0.0f;
            if(newVolume > 1.0f) newVolume = 1.0f;

            volume = newVolume;
            if(currentMusic.ctxData != nullptr) {
                SetMusicVolume(currentMusic, volume);
            }
            
        }
    }
    if(handlePos.x >= rec.x + rec.width - 5) handlePos.x = rec.x + rec.width - 5;
    if(handlePos.x <= rec.x + 5) handlePos.x = rec.x + 5;
    DrawCircleV(handlePos, 8, DARKGRAY);

    DrawText(TextFormat("%d%%", (int) std::round(volume * 100)), rec.x + rec.width + 10, rec.y, 20, DARKGRAY);
}