#include "../include/session.h"
#include "../include/cantp.h"
#include <iostream>
#include <unistd.h> 

using namespace std;

int main() {
    Session session;
    CanFrame frame;
    frame.id = 0x7df; 
    frame.dlc = 8;    
    frame.data[0] = 0x30;
    frame.data[1] = 0x00;
    frame.data[2] = 0x14;
    frame.data[3] = 0x00;
    frame.data[4] = 0x00;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;

    

    Cantp cantp; 
    cantp.decode(frame); 

    return 0;
}
