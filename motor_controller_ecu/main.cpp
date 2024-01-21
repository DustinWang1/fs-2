#include "mbed.h"
#include "CO.h"
#include <bitset>
#include <cstdint>
/*
THIS HERE IS THE CODE TO SEND APPS SENSOR DATA TO OUR MOTOR CONTROLLER VIA CAN
IT ALSO SENDS THE DATA TO THE DASH DISPLAY
*/


CAN can(D10, D2);
AnalogIn analogIn(A0);

int voltage; //HE sensor input calculated

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

int main() {
    //Create Node 
    CANOpenNode canHandle(can, 0x1);

    //Separate thread handles received messages
    eventDispatcher.start(callback(&canHandle.TPDOqueue, &EventQueue::dispatch_forever));

    while(1) {
        canHandle.sendRPDOs(analogIn);
        wait_us(10000);
    }
}
