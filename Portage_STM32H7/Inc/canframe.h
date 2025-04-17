#ifndef CANFRAME_H
#define CANFRAME_H

#include <stdint.h>




struct CanFrame
{
    uint32_t can_id;
    uint8_t dlc;
    uint8_t data[8]  ;
};




#endif // CANFRAME_H
