#include "wordle.hpp"

size_t leaderboardCallback(void * contents, size_t size, size_t number, std::string * result) {
    size_t totalSize = size * number;
    result->append((char *) contents, totalSize);
    return totalSize;
}


void Wordly::loadLeaderboard(void) const {
    CURL * curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("HTTP request failed");
    }
    std::string buffer;
    CURLcode result;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, leaderboardCallback);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/leaderboard");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, & buffer);

    result = curl_easy_perform(curl);

    if(result != CURLE_OK) {
        throw std::runtime_error(std::string("Error: " + std::to_string((int) result)));
    }
    ParserJSON ps ("../test.json");
    ps.parse();
    ps.listKeys();
    curl_easy_cleanup(curl);
}