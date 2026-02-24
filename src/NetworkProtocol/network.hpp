#include <ixwebsocket/IXWebSocket.h>
#include "../../json-parser-c/parser.hpp"
struct Packet {
    std::string word = "";
    bool turn = false;
    std::string roomId = "";
    int attempts = 0;
    Packet(std::string_view w, bool t, std::string_view id) : word(w), turn(t), roomId(id) {}

    Packet() {}
};

class NetworkManager {
    public: 

        Packet packet; 
    NetworkManager() {}
    bool connected(void) const;
    void disconnect(void);
    void sendmsg(const std::string & msg);

    bool getStatus(void) const;
    void setOnWord(std::function<void(const std::string &)> callback);
    void connect(const std::string & addr);

    void sendGamePacket(const std::string & str);
    void receive(void);


         private:
             std::function<void(const std::string &)> onWord;
    ix::WebSocket _socket;
    bool _connected = false;
    bool gameStarted = false;
    ParserJSON parser;
};