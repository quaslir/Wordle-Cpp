#include "network.hpp"
#include <iostream>
#include <thread>

void NetworkManager::connect(const std::string & addr) {
_socket.setUrl(addr);

_socket.setOnMessageCallback([this] (const ix::WebSocketMessagePtr & msg) {
    if(msg->type == ix::WebSocketMessageType::Open) {
        _connected = true;
    }
});

_socket.start();
}

bool NetworkManager::connected(void) const {return this->_connected;}

void NetworkManager::sendmsg(const std::string & msg) {
   if(_connected) {
    _socket.send(msg);
   }
}

void NetworkManager::disconnect(void) {
    _socket.send("STOP");
    _socket.stop();
    this->_connected = false;
}

void NetworkManager::receive(void) {
    if(!this->_connected) return;

    _socket.setOnMessageCallback([this] (const ix::WebSocketMessagePtr & msg) {
        if(msg->type == ix::WebSocketMessageType::Message) {
            std::stringstream ss (msg->str);
            parser.parse(ss);
            
            auto word = parser.getValue<std::string>("word");
            auto turn = parser.getValue<bool>("turn");
            auto id = parser.getValue<std::string>("roomId");

            if(word.has_value() && turn.has_value() && id.has_value()) {
                parser.print();
                packet = Packet(word.value(), turn.value(), id.value());
                gameStarted = true;
            }
            parser.clear();
        }
    });
}

bool NetworkManager::getStatus(void) const{
return gameStarted;
}
