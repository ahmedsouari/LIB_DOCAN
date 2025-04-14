#ifndef CANTP_H
#define CANTP_H

#include <cstdint>
#include <string>
#include <linux/can.h>
#include "idocan.h"
#include "canframe.h"



#define ISOTP_FC_CTS 0x00

#define MAX_PAYLOAD_SIZE 4095
extern uint8_t payload[MAX_PAYLOAD_SIZE];
#define MAX_RESPONSE_SIZE 4095
extern uint8_t response[MAX_RESPONSE_SIZE];


// uint8_t response[MAX_RESPONSE_SIZE] = {};

enum State
{
    IDLE,
    WAIT_DATA,
    SEND_CF,
    FINISHED,
    ERROR
};
/*to test !!!!*/

class CanTp
{
public:
    CanTp();
    ~CanTp();

    /*============================================================ */
    void receiveRequest(CanFrame &frame);
    uint16_t getRequestLength(const CanFrame &frame);
    void decode(CanFrame &frame, uint8_t *payload);
    void encode(const uint8_t *payload, uint16_t length); 


};

#endif
