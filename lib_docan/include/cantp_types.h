#ifndef CANTP_TYPES_H
#define CANTP_TYPES_H

enum PciType {
    SingleFrame = 0x0,
    FirstFrame = 0x1,
    ConsecutiveFrame = 0x2,
    FlowControlFrame = 0x3
};

#endif
