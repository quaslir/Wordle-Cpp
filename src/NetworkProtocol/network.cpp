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
    _socket.stop();
    this->_connected = false;
}


