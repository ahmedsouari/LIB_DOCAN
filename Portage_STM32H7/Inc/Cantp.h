/*
 * Cantp.h
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#ifndef CANTP_H_
#define CANTP_H_

#include <cstdint>
#include "canframe.h"

enum PciType {
	SingleFrame = 0x0,
	FirstFrame = 0x1,
	ConsecutiveFrame = 0x2,
	FlowControlFrame = 0x3
};

#define _FC_CTS 0x00

#define MAX_PAYLOAD_SIZE 4095
extern uint8_t payload[MAX_PAYLOAD_SIZE];
#define MAX_RESPONSE_SIZE 4095
extern uint8_t response[MAX_RESPONSE_SIZE];

class Cantp {
public:
	Cantp();
	void receiveRequest(CanFrame &frame);
	uint16_t getRequestLength(const CanFrame &frame);
	void decode(CanFrame &frame, uint8_t *payload);
	void encode(const uint8_t *payload, uint16_t length);

	virtual ~Cantp();

};

#endif /* CANTP_H_ */
