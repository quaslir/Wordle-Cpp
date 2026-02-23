#include <ixwebsocket/IXWebSocket.h>

class NetworkManager {
     private:
    ix::WebSocket _socket;
    bool _connected = false;
    public:
    NetworkManager() {}

    bool connected(void) const;
    void disconnect(void);
    void sendmsg(const std::string & msg);


    void connect(const std::string & addr);


};