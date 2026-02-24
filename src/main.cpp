#include "wordle.hpp"
int main(int argc, char * argv[]) {
    std::ifstream file;
    if(argc < 2) {
          file.open("../sgb-words.txt");
          if(!file.is_open()) {
            file.open(argv[1]);
            if(!file.is_open()) {
                std::cerr << "No available dictionary was found" << std::endl;
            }
          }
    }
    else {
        file.open(argv[1]);
        if(!file.is_open()) {
                std::cerr << "No available dictionary was found" << std::endl;
            }
    }

    Wordly wordly (file);
    InitWindow(540, 730, "Worldy-C++");
    SetTargetFPS(120);
    bool autoplay = wordly.getAutoplayStatus();
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(wordly.config.bg_color);
       wordly.play();
        EndDrawing();
}
CloseWindow();
    return 0;
} 