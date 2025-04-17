/*
 * Linklayer.h
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#ifndef LINKLAYER_H_
#define LINKLAYER_H_
#include "canframe.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

class Linklayer {
public:
	Linklayer();
	~Linklayer();
	/* Method to initialise CANFD */

	uint8_t sendFrame(const CanFrame &frame);
	/* Method to read a CAN frame */
	uint8_t readFrame(CanFrame &frame);
	/* Method to send a CAN frame */
	void Init_CAN();


private:
	// Add private members if needed
	FDCAN_HandleTypeDef hfdcan1;


};

#ifdef __cplusplus
}
#endif

#endif /* LINKLAYER_H_ */
