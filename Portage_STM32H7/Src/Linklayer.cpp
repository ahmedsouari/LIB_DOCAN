/*
 * Linklayer.cpp
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#include <Linklayer.hpp>

#include "canframe.h"
#include "main.h"
#include "stm32h7xx_hal_fdcan.h" // Include the FDCAN HAL header
#include "stm32h7xx_hal.h"       // Ensure the main HAL header is included
#include <stdio.h>
extern uint8_t RxData[8];
extern uint8_t TxData1[8];
Linklayer::Linklayer() {
	// TODO Auto-generated constructor stub

}

Linklayer::~Linklayer() {
	// TODO Auto-generated destructor stub
}

uint8_t Linklayer::sendFrame(const CanFrame &frame) {
	FDCAN_TxHeaderTypeDef TxHeader1;
	TxHeader1.Identifier = 0x11;
	TxHeader1.IdType = FDCAN_STANDARD_ID;
	TxHeader1.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader1.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader1.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader1.FDFormat = FDCAN_FD_CAN;
	TxHeader1.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader1.MessageMarker = 0;

	/*Fill Buffer */
	for (int i = 0; i < 8; ++i) {
		TxData1[i] = frame.data[i];
	}
	printf("send frame from link layer 1\n");
	/* Send Frame */
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader1, TxData1)
			!= HAL_OK) {
		Error_Handler();
	}
	printf("send frame from link layer 2\n");
	HAL_Delay(10);
	return 0;
}

uint8_t Linklayer::readFrame(CanFrame &frame) {

	printf("readFrame from link layer\n");
	for (int i = 0; i < 8; i++) {
		printf("0x%02X ", RxData[i]);
		frame.data[i] = RxData[i];
		RxData[i] = 0;
	}

	printf("\n ");

	return 0;

}
