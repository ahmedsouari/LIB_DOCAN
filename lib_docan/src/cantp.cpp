#include "../include/cantp.h"
#include "../include/cantp_types.h" // Include for PciType enum
#include <iostream>
#include <vector>

using namespace std; // Added namespace

void Cantp::encode() {
    // en cours !!!!
}

void Cantp::decode(const CanFrame& frame) {
    cout << "Decoding CAN frame with ID: 0x" << hex << frame.id << dec << endl;

    if (frame.dlc < 1) {
        cerr << "Invalid CAN frame: DLC is less than 1." << endl;
        return;
    }

    PciType pci_type = static_cast<PciType>(frame.data[0] >> 4); // Extraction PCI
    switch (pci_type) {
        case SingleFrame: { // Single Frame
            uint8_t payload_length = frame.data[0] & 0x0F; 
            cout << "Single Frame detected. Payload length: " << static_cast<int>(payload_length) << endl;
            cout << "Data: [";
            for (int i = 1; i <= payload_length; ++i) {
                cout << "0x" << hex << static_cast<int>(frame.data[i]);
                if (i < payload_length) cout << ", ";
            }
            cout << "]" << endl;
            break;
        }
        case FirstFrame: { // First Frame
            uint16_t total_length = ((frame.data[0] & 0x0F) << 8) | frame.data[1]; // Extract total length
            cout << "First Frame detected. Total length: " << total_length << endl;
            cout << "Data: [";
            for (int i = 2; i < frame.dlc; ++i) {
                cout << "0x" << hex << static_cast<int>(frame.data[i]);
                if (i < frame.dlc - 1) cout << ", ";
            }
            cout << "]" << endl;
            break;
        }
        case ConsecutiveFrame: { // Consecutive Frame
            uint8_t sequence_number = frame.data[0] & 0x0F; 
            cout << "Consecutive Frame detected. Sequence number: " << static_cast<int>(sequence_number) << endl;
            cout << "Data: [";
            for (int i = 1; i < frame.dlc; ++i) {
                cout << "0x" << hex << static_cast<int>(frame.data[i]);
                if (i < frame.dlc - 1) cout << ", ";
            }
            cout << "]" << endl;
            break;
        }
        case FlowControlFrame: { // Flow Control Frame
            uint8_t flow_status = frame.data[0] & 0x0F;
            uint8_t block_size = frame.data[1];
            uint8_t st_min = frame.data[2];
            cout << "Flow Control Frame detected. Flow status: " << static_cast<int>(flow_status)
                 << ", Block size: " << static_cast<int>(block_size)
                 << ", STmin: " << static_cast<int>(st_min) << endl;
            break;
        }
        default:
            cerr << "Unknown PCI type: 0x" << hex << static_cast<int>(pci_type) << dec << endl;
            break;
    }
}