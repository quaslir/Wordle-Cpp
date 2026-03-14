#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

struct MusicManager {
    float volume = 0.7f;
    std::vector<std::string> musicPaths;
    Music currentMusic;
    size_t currentIndex = 0;

    MusicManager() {}
    void loadAllFromFolder(const std::string & path);
    void setCurrentMusic(void);
    void updateMusic(void);

    void drawSlider(const Rectangle & rec);
    ~MusicManager();
};
