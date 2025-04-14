#ifndef CANFRAME_H
#define CANFRAME_H

#include <cstdint>
#include <array>

struct CanFrame
{
    uint32_t can_id;
    uint8_t dlc;
    uint8_t data[8] __attribute__((aligned(8))) ;
};



#endif // CANFRAME_H


