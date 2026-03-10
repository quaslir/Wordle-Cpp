#include "leaderboard.hpp"
#include "button.hpp"
#include "network.hpp"
#include "post.hpp"
void Leaderboard::loadLeaderboard(void) {

    std::string buffer = getRequest("http://localhost:3000/leaderboard");
    ParserJSON ps;
    std::stringstream ss (buffer);

    ps.parse(ss);
            
    size_t size = ps.getSize();
    for(size_t i = 0; i < size; i++) {
        auto x = ps.getObject(std::to_string(i)).value();
        std::string username = x.getValue<std::string>("username").value();
        size_t xp = x.getValue<size_t>("xp").value();
        leaderboard.push_back({username, xp});
    }
    std::sort(leaderboard.begin(), leaderboard.end(), [](const std::pair<std::string, size_t> & first, 
    const std::pair<std::string, size_t> & second) {
        return first.second > second.second;
    });
    leaderboardLoaded = true;

}


void Leaderboard::updateLeaderboard(const std::string & username, const size_t xp) {
CURL * curl = curl_easy_init();

if(!curl) {
        throw std::runtime_error("HTTP request failed");
}
ParserJSON data;
data.insert<std::string>("username", username);

data.insert<std::string>("xp", std::to_string((int) xp));

std::string dataFormatted = data.toString();
struct curl_slist * headers = NULL;
headers = curl_slist_append(headers, "Content-Type: application/json");
curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/update-leaderboard");
curl_easy_setopt(curl, CURLOPT_HTTPHEADER,headers);
curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataFormatted.c_str());


curl_easy_perform(curl);

curl_easy_cleanup(curl);

}

void Leaderboard::renderLeaderboard(void) {
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    drawLogo();
int screenWidth = GetScreenWidth();
int panelWidth = 450;
int rowHeight = 45;
int x = (screenWidth - panelWidth) / 2;
int y = 150;
DrawRectangle(x, y - 40, panelWidth, 35, Fade(GRAY, 0.3f));
DrawText("RANK", x + 10, y - 33, 20, GOLD);
DrawText("PLAYER", x + 100, y - 33, 20, GOLD);
DrawText("TOTAL XP", x + panelWidth - 110, y - 33, 20, GOLD);

    for(int i = 0; i < leaderboard.size(); i++) {
        Color rowColor  = (leaderboard[i].first == getUsername()) ? GREEN : RAYWHITE;
        int currentY = y + (i * rowHeight);

        if(i % 2 == 0) {
            DrawRectangle(x, currentY - 5, panelWidth, rowHeight - 5, Fade(WHITE, 0.05f));
        }

        Color rankColor = i == 0 ? GOLD : i == 1 ? LIGHTGRAY : i == 2 ? ORANGE : WHITE;
        DrawText(TextFormat("%02d", i + 1), x + 15, currentY, 22, i == 0 || i == 1 || i == 2 ? rankColor : rowColor);  

        DrawText(leaderboard[i].first.c_str(), x + 100, currentY, 22, rowColor);
        std::string xpStr = std::to_string((int) leaderboard[i].second);
        int textWidth = MeasureText(xpStr.c_str(), 22);

        DrawText(xpStr.c_str(), x + panelWidth  - 15 - textWidth, currentY, 22, rowColor);

    }

 
    Rectangle rec = {(float) screenWidth / 2 - 60, (float) GetScreenHeight() - 80, 120, 40};
     Color colorBtn = DARKGRAY;
    if(CheckCollisionPointRec(GetMousePosition(), rec)) {
        colorBtn = LIGHTGRAY;
    }
       Button btn (rec, colorBtn, "Back");
    btn.drawBtn();
    if(btn.checkClick(GetMousePosition())) {
        leaderboardLoaded = false;
        leaderboard.clear();
        changeState();
    }

}

void Leaderboard::receiveUsersXp(const std::string & username) {
std::string buffer;
    ParserJSON data;
data.insert<std::string>("username", username);

std::string dataFormatted = data.toString();
buffer = postRequest("http://localhost:3000/getUsersXp", dataFormatted);
    ParserJSON ps;
    std::stringstream ss (buffer);
    ps.parse(ss);

    if(!ps.exists("xp")) return;

    auto x = ps.getValue<int>("xp");

    if(!x.has_value()) return;
    getXp(x.value());

}


