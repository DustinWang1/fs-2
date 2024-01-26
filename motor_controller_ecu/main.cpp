#include "mbed.h"
#include "CO.h"
#include <bitset>
#include <chrono>
#include <cstdint>
#include <functional>
/*
THIS HERE IS THE CODE TO SEND APPS SENSOR DATA TO OUR MOTOR CONTROLLER VIA CAN
IT ALSO SENDS THE DATA TO THE DASH DISPLAY
*/
Timer implausibilityTimer;

int voltage; //HE sensor input calculated

//Native CAN and canHandle
CAN *can;
CANOpenNode *canHandle;

//EventQueue TPDOqueue(64+11);
Thread eventDispatcher;
Thread RPDOsend;

/*
void handleTPDO() {
    // Read the CAN message
    CANMessage msg;
    if (can->read(msg)) {
        // Example: Print the received message
        printf("Received CAN message ID: 0x%X, Length: %d, Data: ", msg.id, msg.len);
        for (int i = 0; i < msg.len; i++) {
            printf("%02X ", msg.data[i]);
        }
        printf("\n");

        TPDOqueue.call(&CANOpenNode::decodeTPDO, msg, canHandle);
    }
}
*/

#ifdef HELLO1234555
bool checkPlausibility(AnalogIn HE1, AnalogIn HE2) {
    //Calculate angle from voltage

    if(false/*If angle difference greater than 10% implausibility occurs*/) {
        if((implausibilityTimer.elapsed_time()).count() == 0) {
            implausibilityTimer.start();
            return false;
        } else if((implausibilityTimer.elapsed_time()).count() >= 100000){
            //KILL THE SYSTEM
            //maybe a can signal
            return false;
        }
    }
    //Start timer return false
    return true;
}
#endif

void initIO() {
    can = new CAN(PA_11, PA_12, 500000);
    canHandle = new CANOpenNode(can, 0x1);
}

int main() {  
    initIO();
    
    AnalogIn HE1(A0);
    AnalogIn HE2(A1);

    can->reset();
    can->mode(CAN::Normal);
    

    //Separate thread handles received messages
    //eventDispatcher.start(callback(&TPDOqueue, &EventQueue::dispatch_forever));

    //can->attach(&handleTPDO, CAN::RxIrq);
    while(1) {
        //if(checkPlausibility(HE1, HE2)) {
        canHandle->sendRPDOs(HE1, HE2);
        //}
        wait_us(10000);
    }
}




