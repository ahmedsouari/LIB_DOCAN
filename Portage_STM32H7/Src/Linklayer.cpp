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
#include "FreeRTOS.h"
#include "queue.h"
extern FDCAN_HandleTypeDef hfdcan1;
extern QueueHandle_t xCanFrameQueue ;
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

		/* Send Frame */
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, frame.data) != HAL_OK)
	{
		return 1;
		Error_Handler();
	}

	return 0U;
}
uint8_t Linklayer::readFrame(CanFrame* frame) {

	uint8_t ret =0U ;
	if (   frame == NULL )
	{
		ret = 1U ;
	}
	else
	{
		if ( pdPASS !=  xQueueReceive(  xCanFrameQueue ,frame->data , 0U )  )
		{
			ret = 2U ;
		}
	}
	return ret ;
}


