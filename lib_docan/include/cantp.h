#ifndef CANTP_H
#define CANTP_H

#include "../include/session.h"
#include "../include/cantp_types.h" // Include for PciType enum
using namespace std;

class Cantp {
public:
    void encode(); 
    void decode(const CanFrame& frame); 
};

#endif


