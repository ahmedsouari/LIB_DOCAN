/*
 * Cantp.cpp
 *
 *  Created on: Apr 14, 2025
 *      Author: asouari@actia.local
 */

#include "Cantp.h"
#include <cstring>
#include "canframe.h"
#include "Linklayer.hpp"
#include <stdio.h>

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
		printf("Consecutive Frame does not define total length.\n");
		return 0;
	}
	case FlowControlFrame: {
		printf(" Flow Control Frame does not define total length.\n");
		return 0;
	}
	default: {
		//cerr << "Unknown PCI type: 0x" << hex << static_cast<int>(pciType) << endl;
		return 0;
	}
	}
}

void Cantp::decode(CanFrame &frame, uint8_t *payload) {

	uint8_t pciType = frame.data[0] >> 4U;
	uint16_t totalLength = 0;
	uint16_t receivedLength = 0;
	switch (pciType) {
	case SingleFrame: {
	uint8_t payloadLength = frame.data[0] & 0x0F;
	//cout << "Single Frame received." << endl;

	memcpy(payload, frame.data + 1, payloadLength);
	receivedLength = payloadLength;

		//cout << "Payload (Hex): ";
		for (uint8_t i = 0; i < receivedLength; ++i) {
			//cout << "0x" << hex << static_cast<int>(payload[i]) << " ";
		}
		//cout << dec << endl;
		break;
		}
		case FirstFrame: {
		//cout << "First Frame received." << endl;
		totalLength = ((frame.data[0] & 0x0F) << 8) | frame.data[1];
		//cout << "Total Length: " << totalLength << endl;

		uint16_t payloadLength = totalLength > 6 ? 6 : totalLength;
		memcpy(payload, frame.data + 2, payloadLength);
		receivedLength += payloadLength;

		//cout << "Payload (Hex): ";
		for (uint16_t i = 0; i < receivedLength; ++i) {
			//cout << "0x" << hex << static_cast<int>(payload[i]) << " ";
		}
		//cout << dec << endl;

		Linklayer linklayer;
		while (receivedLength < totalLength) {
			struct CanFrame nextFrame;

			if (linklayer.readFrame(nextFrame) != 0) {
				//cerr << "Error reading consecutive frame." << endl;
				return;
			}

			if ((nextFrame.data[0] >> 4) != ConsecutiveFrame) {
				//cerr << "Unexpected frame type received." << endl;
				return;
			}

			// uint8_t seqId = nextFrame.data[0] & 0x0F;
			// cout << "Consecutive Frame received. Sequence ID: " << static_cast<int>(seqId) << endl;

			uint16_t remainingLength = totalLength - receivedLength;
			uint16_t payloadLength = remainingLength > 7 ? 7 : remainingLength;
			memcpy(payload + receivedLength, nextFrame.data + 1, payloadLength);
			receivedLength += payloadLength;
		}
		break;
	}
	default: {
		//cerr << "Unsupported or unknown PCI type: 0x" << hex << static_cast<int>(pciType) << endl;
		break;
	}
	}
	//cout << "Payload (Hex): ";
	for (uint16_t i = 0; i < receivedLength; ++i) {
		//cout << "0x" << hex << static_cast<int>(payload[i]) << " ";
	}
	//cout << dec << endl;
	//cout << "=====================================" << endl;

	/*Message Reconstitution */
	//cout << "Final Payload (Hex): ";
	for (uint16_t i = 0; i < receivedLength; ++i) {
		payload[i] = payload[i]; // Copy data to the payload array
		//cout << "0x" << hex << static_cast<int>(payload[i]) << " ";
	}
	//cout << dec << endl;
}

void Cantp::receiveRequest(CanFrame &frame) {
	Linklayer linkLayer; // Create a LinkLayer instance

	if (linkLayer.readFrame(frame) != 0) {
		//cerr << "Erreur lors de la lecture de la trame CAN." << endl;
		return;
	}
	// send flow control frame
	CanFrame fcFrame;

	fcFrame.data[0] = FlowControlFrame | _FC_CTS; // PCI type + flow control command
	fcFrame.data[1] = 0x00; // Block size
	fcFrame.data[2] = 0x00; // Separation time
	// Send the flow control frame
	if (linkLayer.sendFrame(fcFrame) != 0) {
		//cerr << "Error sending Flow Control Frame." << endl;
		return;
	}
	//cout << "Flow Control Frame sent." << endl;

	// Extract the message length from the first frame
	//uint16_t messageLength = getRequestLength(frame);
	//cout << "Message length to receive: " << messageLength << " bytes" << endl;

	decode(frame, payload);
}

void Cantp::encode(const uint8_t *payload, uint16_t length) {
	Linklayer linkLayer;

	if (length <= 7) {
		/*Send Single Frame*/
		CanFrame frame = { };
		frame.can_id = 0x123U;
		frame.dlc = static_cast<uint8_t>(length + 1);
		frame.data[0] = SingleFrame | (length & 0x0F);
		memcpy(frame.data + 1, payload, length);

		// Send the frame
		if (linkLayer.sendFrame(frame) != 0) {
			//cerr << "Error sending Single Frame." << endl;
		}
	} else {
		/*Send First Frame*/
		CanFrame firstFrame = { };
		firstFrame.can_id = 0x123; /*to change according to uds spec*/
		firstFrame.dlc = 8;
		firstFrame.data[0] = FirstFrame | ((length >> 8) & 0x0F);
		firstFrame.data[1] = length & 0xFF;
		memcpy(firstFrame.data + 2, payload, 6);

		/*Send First Frame*/
		if (linkLayer.sendFrame(firstFrame) != 0) {
			//cerr << "Error sending First Frame." << endl;
			return;
		}

		/*Consecutive Frame*/
		uint16_t remainingLength = length - 6;
		const uint8_t *remainingData = payload + 6;
		uint8_t seqId = 1;

		while (remainingLength > 0) {
			CanFrame consecutiveFrame = { };
			consecutiveFrame.can_id = 0x123;
			consecutiveFrame.dlc = 8;
			consecutiveFrame.data[0] = ConsecutiveFrame | (seqId & 0x0F);
			uint8_t payloadLength = (remainingLength > 7) ? 7 : remainingLength;
			memcpy(consecutiveFrame.data + 1, remainingData, payloadLength);

			/*Send the consecutive frame*/
			if (linkLayer.sendFrame(consecutiveFrame) != 0) {
				//cerr << "Error sending Consecutive Frame." << endl;
				return;
			}

			remainingLength -= payloadLength;
			remainingData += payloadLength;
			seqId = (seqId + 1) & 0x0F; // Sequence ID wraps around after 15
		}
	}
}

