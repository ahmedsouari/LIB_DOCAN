#ifndef SESSION_H
#define SESSION_H

#include <cstdint>
//struct CanFrame
struct CanFrame {
    uint32_t id;      
    uint8_t dlc;       
    uint8_t data[8];   
};

class Session {
public:
    void write(const CanFrame& frame); 
    void read(); 
    void shutdown(); 
};

#endif
