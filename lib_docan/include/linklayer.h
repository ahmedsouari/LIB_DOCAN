#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <linux/can.h>
#include <cstdint>
#include <string>
#include "canframe.h" 

using namespace std;

class LinkLayer {
public:
    LinkLayer(const std::string &interface);  
    ~LinkLayer(); 

    uint8_t readFrame(struct CanFrame &frame);
    uint8_t sendFrame(const CanFrame &frame);
    bool init(string _interface, uint8_t bitrate);

private:
    int _socket_fd;
    string _interface;
    uint64_t _bitrate;
};

#endif
