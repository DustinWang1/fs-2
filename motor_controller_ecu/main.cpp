#include "mbed.h"
#include "CO.h"
#include <bitset>
#include <cstdint>
/*
THIS HERE IS THE CODE TO SEND APPS SENSOR DATA TO OUR MOTOR CONTROLLER VIA CAN
IT ALSO SENDS THE DATA TO THE DASH DISPLAY
*/

AnalogIn analogIn(A0);

//Native CAN 
CAN can(D10, D2);
//Custom CANStack handle
CANOpenNode canHandle(can, 0x1);

int voltage; //HE sensor input calculated

EventQueue TPDOqueue(64+11);

Thread eventDispatcher;
Thread RPDOsend;

/* TEST FUNCTION
void printCharArrayAsBinary(char* charArray, int size) {
    for (int i = 0; i < size; ++i) {  
      for(int j = 0; j < 8; j++) {
      std::cout << ((charArray[i] & (1 << (7-j))) ? "1" : "0");
      }
      std::cout << ' ';
    }
  std::cout << std::endl;
}
*/
void handleTPDO() {
    // Read the CAN message
    CANMessage msg;
    if (can.read(msg)) {
        // Example: Print the received message
        printf("Received CAN message ID: 0x%X, Length: %d, Data: ", msg.id, msg.len);
        for (int i = 0; i < msg.len; i++) {
            printf("%02X ", msg.data[i]);
        }
        printf("\n");

        TPDOqueue.call(&CANOpenNode::decodeTPDO, msg, canHandle);
    }
}

int main() {


    //Separate thread handles received messages
    eventDispatcher.start(callback(&TPDOqueue, &EventQueue::dispatch_forever));

    can.attach(&handleTPDO, CAN::RxIrq);
    while(1) {
        canHandle.sendRPDOs(analogIn);
        wait_us(10000);
    }
}




