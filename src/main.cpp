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

   
    InitWindow(700, 900, "Worldy-C++");
    InitAudioDevice();
     Wordly wordly (file);
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
       wordly.play();
        EndDrawing();
}
CloseWindow();
    return 0;
} 