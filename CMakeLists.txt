cmake_minimum_required(VERSION 3.10)
project(Wordle-Cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++")
endif()

find_package(CURL REQUIRED)


include(FetchContent)

FetchContent_Declare(
    ixwebsocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT_TAG master
)

set(IXWEBSOCKET_INSTALL OFF CACHE BOOL "" FORCE)
set(USE_TLS ON CACHE BOOL "" FORCE) 

FetchContent_MakeAvailable(ixwebsocket)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/raylib)
add_subdirectory(vendor/json-parser)
set(SOURCES
    src/main.cpp        
    src/wordle.cpp
    src/button.cpp
    src/keyboard.cpp
    src/timer.cpp
    src/wordleDraw.cpp
    src/wordleUsername.cpp
    src/textBox.cpp
    src/drawStats.cpp
    src/leaderboard.cpp
    src/network.cpp
    src/wordleInit.cpp
    src/autoplay.cpp
    src/wordleUpdate.cpp
    src/settings.cpp
    src/post.cpp
    src/hint.cpp
    src/music.cpp

)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE vendor/raylib/src vendor/json-parser/)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} json-parser raylib CURL::libcurl ixwebsocket)


if(APPLE) 
    target_link_libraries(${PROJECT_NAME} c++) 
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")
endif()