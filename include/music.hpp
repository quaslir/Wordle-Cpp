#include "raylib.h"
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
struct MusicManager {
    float volume = 0.7f;
    std::vector<std::string> backgroundTracks;
    Music currentMusic = {0};
    size_t currentIndex = 0;
    std::unordered_map<std::string, std::string> eventTracks;
    bool activePlaylist = false;
    std::string currentTrackPath = "";
    MusicManager() {}
    void loadAllFromFolder(const std::string & path);
    void setCurrentMusic(void);
    void updateMusic(void);
    void playFromPlaylist(const std::string & key);
    void drawSlider(const Rectangle & rec);
    ~MusicManager();
};
