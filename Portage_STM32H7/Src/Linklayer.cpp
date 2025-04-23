/*
 * Linklayer.cpp
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#include <Linklayer.hpp>
#include "canframe.h"
#include "main.h"
#include "stm32h7xx_hal_fdcan.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>

extern FDCAN_HandleTypeDef hfdcan1;

Linklayer::Linklayer() {
	// TODO Auto-generated constructor stub

}

Linklayer::~Linklayer() {
	// TODO Auto-generated destructor stub
}

uint8_t Linklayer::sendFrame(CanFrame &frame) {

	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = frame.can_id;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	LOG("send frame from link layer 1\n");
	/* Send Frame */
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, frame.data)
			!= HAL_OK) {
		Error_Handler();
	} LOG("send frame from link layer 2\n");
	HAL_Delay(10);
	return 0;
}
uint8_t Linklayer::readFrame(CanFrame* frame) {

    if (flag == true) {
        //printf("readFrame from link layer\n");
        for (int i = 0; i < 8; i++) {
            LOG("0x%02X ", RxData[i]);
            frame->data[i] = RxData[i];
            RxData[i] = 0;
        }
        LOG("\n ");
        HAL_Delay(100);
        return 0;
    } else {
    	HAL_Delay(100);
        return 1;
    }
}


