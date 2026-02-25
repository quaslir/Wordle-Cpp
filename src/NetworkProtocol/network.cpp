#include "network.hpp"
#include <iostream>
#include <thread>
#include <functional>
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
    this->gameStarted = false;
}

void NetworkManager::receive(void) {
    if(!this->_connected) return;

    _socket.setOnMessageCallback([this] (const ix::WebSocketMessagePtr & msg) {
        if(msg->type == ix::WebSocketMessageType::Message) {
            std::stringstream ss (msg->str);
            parser.parse(ss);
            if(!gameStarted) {
            auto word = parser.getValue<std::string>("word");
            auto turn = parser.getValue<bool>("turn");
            auto id = parser.getValue<std::string>("roomId");

            if(word.has_value() && turn.has_value() && id.has_value()) {
                parser.print();
                std::cout << turn.value() << std::endl;
                packet = Packet(word.value(), turn.value(), id.value());
                gameStarted = true;
            }

        }
        else {
            auto word = parser.getValue<std::string>("word");
            auto turn = parser.getValue<bool>("turn");
            if(turn.has_value()) {
                this->packet.turn = turn.value();
            }

            if(word.has_value()) {
                onWord(word.value());
            }

            if(parser.exists("win")) {
                auto winStatus = parser.getValue<bool>("win");
                if(winStatus.has_value()) {
                    packet.win = winStatus.value();
                    if(!packet.win) {
                        auto drawStatus = parser.getValue<bool>("draw");
                        if(drawStatus.has_value()) {
                            packet.draw = drawStatus.value();
                        }
                    }
                    this->_connected = false;
                    this->gameStarted = false;
                }
                
            }
            parser.print();

        }

                    parser.clear();
    }
    });
}

bool NetworkManager::getStatus(void) const{
return gameStarted;
}

void NetworkManager::sendGamePacket(const std::string & str) {
    parser.insert("word",str);
    parser.insert("id", this->packet.roomId);
    std::string formatted = parser.toString();
    if(_connected) {
    _socket.send(formatted);
    }
}

void NetworkManager::setOnWord(std::function<void(const std::string &)> callback) {
onWord = callback;
}
