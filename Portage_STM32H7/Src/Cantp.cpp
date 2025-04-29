/*
 * Cantp.cpp
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#include "Cantp.h"
#include <cstring>
#include "canframe.h"
#include "main.h"
#include "Linklayer.hpp"
#include <stdio.h>
#include "cmsis_os.h"


Cantp::Cantp() {
	// TODO Auto-generated constructor stub
}

Cantp::~Cantp() {
	// TODO Auto-generated destructor stub
}

uint16_t Cantp::getRequestLength(const CanFrame &frame) {

	uint8_t pciType = frame.data[0] >> 4;
	uint16_t totalLength;
	switch (pciType) {
	case SingleFrame: {
		uint8_t payloadLength = frame.data[0] & 0x0F;
		return payloadLength;
	}
	case FirstFrame: {
		totalLength = ((frame.data[0] & 0x0F) << 8) | frame.data[1];
		return totalLength;
	}
	case ConsecutiveFrame: {
		LOG("Consecutive Frame does not define total length.\n");
		return 0;
	}
	case FlowControlFrame: {
		LOG(" Flow Control Frame does not define total length.\n");
		return 0;
	}
	default: {
		LOG(" Unknown PCI type\n");
		return 0;
	 }
   }
}

uint8_t Cantp::decode(CanFrame &frame, uint8_t *payload)
{
	Linklayer linklayer;
	CanFrame nextFrame;

	uint16_t pciType = frame.data[0] >> 4U;
	uint16_t totalLength;
	uint16_t receivedLength = 0;


	switch (pciType)
	{
	 case SingleFrame:
	 {
		uint16_t payloadLength = (frame.data[0] & 0x0F);
		memcpy(payload, frame.data + 1, payloadLength);
		return 0;
		break;
	}
	case FirstFrame:
	{
		/*Total Request Length */
		totalLength = ((frame.data[0] & 0x0F) << 8) | frame.data[1];
		uint16_t payloadLength = totalLength > 6 ? 6 : totalLength;
		memcpy(payload, frame.data + 2, payloadLength);
		/*Update receivedLenght*/
		receivedLength += payloadLength;
		uint16_t timeout=0;
		Linklayer linkLayer;
		CanFrame fcFrame;
		/*send flow control frame*/
		fcFrame.can_id = 0x11;
		fcFrame.data[0] = FlowControlFrame;
		/*Block size*/
		fcFrame.data[1] = 0x00;
		/*Separation time*/
		fcFrame.data[2] = 0x00;

		/* Send the flow control frame */
		if (linkLayer.sendFrame(fcFrame) != 0)
		{
			LOG("Error sending Flow Control Frame.\n");
			return 1;
		}
			LOG("Flow Control Frame sent.\n");

		while ( receivedLength < totalLength )
		{
			if ( linklayer.readFrame(&nextFrame) !=0 )
			{
				timeout++;
				HAL_Delay(1);
				LOG("waiting for next frame ...\n ");
					if (timeout > 1001U)
					{
						LOG("TIME OUT ! \n");
						return 0;
						break;
					}
					else
					{
						LOG("Still %d ms before TIMEOUT ! \n",1000-timeout);
						continue;
					}
			}
			else
			{
				if ((nextFrame.data[0] >> 4) != ConsecutiveFrame)
				  {
					LOG("Invalid Frame Type  ...\n");
					return 1;
					break;
				  }
				else
				{
					uint16_t remainingLength = totalLength - receivedLength;
					uint16_t payloadLength = remainingLength > 7 ? 7 : remainingLength;
					memcpy(payload + receivedLength, nextFrame.data + 1,payloadLength);
					receivedLength += payloadLength;
					timeout=0;
					/* uint16_t seqId = nextFrame.data[0] & 0x0F; */
				}
			}
		}
	}
	default:
	{
		LOG("Unsupported or unknown PCI type \n");
		break;
    }
}


	/* Before UDS LIB INTEGRATION */
	if(receivedLength == totalLength)
	{
	/* Display Message after */
	printf("Final Payload (Hex): ");
	for (uint16_t i = 0; i < receivedLength; ++i) {
		printf("0x%X ", payload[i]);
	}
	printf("\n");
	return 0;
	}
	else
	{
		LOG("Session Aborted ! \n");
		for (uint16_t i = 0; i < receivedLength; ++i)
		{
			payload[i]=0U;
		}
		return 1;
	}
}


void Cantp::receiveRequest(CanFrame &frame) {
	Linklayer linkLayer;
	if (linkLayer.readFrame(&frame) != 0)
	{
		printf("Waiting for valid request ...\n");
	}
	else
	{
		decode(frame, payload);
	}
}

uint8_t Cantp::encode(const uint8_t *response, uint16_t length) {
	Linklayer linkLayer;

	if (length <= 7)
	{
		/*Send Single Frame*/
		CanFrame frame;
		frame.can_id = 0x11U;
		frame.dlc = static_cast<uint8_t>(length + 1);
		frame.data[0] = SingleFrame | (length & 0x0F);
		memcpy(frame.data + 1, response, length);

		/*Send The Frame*/
		if (linkLayer.sendFrame(frame) != 0)
		{
			printf("Error sending Single Frame.\n" );
			return 1;
		}
	}

	else {
		/*Send First Frame*/
	    	CanFrame firstFrame;
	    	firstFrame.can_id = 0x123; /*to change according to uds spec*/
	    	firstFrame.dlc = 8;
	    	firstFrame.data[0] = FirstFrame | ((length >> 8) & 0x0F);
	    	firstFrame.data[1] = length & 0xFF;
	    	memcpy(firstFrame.data + 2, response, 6);

		/*Send First Frame*/
		if (linkLayer.sendFrame(firstFrame) != 0)
		{
			printf("Error sending First Frame.\n");
			return 1;
		}

		/*Consecutive Frame*/
		uint16_t remainingLength = length - 6;
		const uint8_t *remainingData = response + 6;
		uint16_t seqId = 1;

		while (remainingLength > 0)
		{
			CanFrame consecutiveFrame;
			consecutiveFrame.can_id = 0x123U;
			consecutiveFrame.dlc = 8;
			consecutiveFrame.data[0] = ConsecutiveFrame | (seqId & 0x0F);
			uint16_t payloadLength = (remainingLength > 7) ? 7 : remainingLength;
			memcpy(consecutiveFrame.data + 1, remainingData, payloadLength);

			/*Send the consecutive frame*/
			if (linkLayer.sendFrame(consecutiveFrame) != 0)
			{
				LOG("Error sending Consecutive Frame.");
				return 1;
			}

			remainingLength -= payloadLength;
			remainingData += payloadLength;
			seqId = (seqId + 1) & 0x0F; // Sequence ID wraps around after 15
		}
	}
	return 0;
}

