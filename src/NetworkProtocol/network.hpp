#include <ixwebsocket/IXWebSocket.h>
#include "../../json-parser-c/parser.hpp"
struct Packet {
    std::string word = "";
    bool turn = false;
    std::string roomId = "";

    Packet(std::string_view w, bool t, std::string_view id) : word(w), turn(t), roomId(id) {}

    Packet() {}
};

class NetworkManager {
     private:
    ix::WebSocket _socket;
    bool _connected = false;
    bool gameStarted = false;
    ParserJSON parser;
    public:
        Packet packet; 
    NetworkManager() {}

    bool connected(void) const;
    void disconnect(void);
    void sendmsg(const std::string & msg);

    bool getStatus(void) const;

    void connect(const std::string & addr);


    void receive(void);

};