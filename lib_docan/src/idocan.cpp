#include "../include/idocan.h"
#include <iostream>
#include "../include/cantp.h" 


using namespace std;

IDoCAN::IDoCAN() 
{
    /*Constructor of IDoCAN class*/
}

IDoCAN::~IDoCAN()
{
    /*Destructor of IDoCAN class*/
}

void IDoCAN::DoCANIn()
{
    /* Receive the Response (that would be sent to the client)  after being processed by the uds lib*/

}

void IDoCAN::DoCANOut()
{
   /*Forwarding the reconsituted message to the uds lib */

}
