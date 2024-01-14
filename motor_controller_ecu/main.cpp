#include "mbed.h"
#include <bitset>
#include <cstdint>
/*
THIS HERE IS THE CODE TO SEND APPS SENSOR DATA TO OUR MOTOR CONTROLLER VIA CAN
IT ALSO SENDS THE DATA TO THE DASH DISPLAY
*/


const int CAN_MESSAGE_MAX_SIZE = 11+64;
CAN can(D10, D2);
AnalogIn analogIn(A0);

int voltage; //HE sensor input calculated



//All TPDO stuff
EventQueue TPDOqueue(32 * CAN_MESSAGE_MAX_SIZE);
Thread dispatcher;

//All RPDO params
const unsigned int THROTTLE_DEMAND_COB_ID = 0x186;
const unsigned int MAX_CURRENTS_COD_ID = 0x286;

/*
Throttle demand params
*/
int16_t torqueDemand = 10; //16 bit
int16_t maxSpeed = 10; //16 bit
//four bit missing
int8_t powerReady = 1;// 1bit
//one bit missing
int8_t reverseState = 0;//1bit
int8_t  forwardState = 1;//1bit
//four bits missing
int8_t MBB_alive = 12; //4 bit
//16 bits missing

/*
Max Currents params
*/
int16_t chargeCurrentLimit;
int16_t dischargeCurrentLimit;

/*
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

void sendThrottleDemand() {
    char nah[8];
    nah[0] = (torqueDemand & 0xFF00) >> 8;
    nah[1] = (torqueDemand & 0x00FF);
    nah[2] = (maxSpeed & 0xFF00) >> 8;
    nah[3] = (maxSpeed & 0x00FF);
    nah[4] = ((powerReady | 0x00) << 3) | ((reverseState | 0x00) << 1) | (forwardState | 0x00);
    nah[5] = MBB_alive;

    can.write(CANMessage(THROTTLE_DEMAND_COB_ID, nah));
}

void sendMaxCurrents() {
    //same method as the last one
    char nah[8];
    nah[0] = (chargeCurrentLimit & 0xFF00) >> 8;
    nah[1] = (chargeCurrentLimit & 0x00FF);
    nah[2] = (dischargeCurrentLimit & 0xFF00) >> 8;
    nah[3] = (dischargeCurrentLimit & 0x00FF);

    can.write(CANMessage(MAX_CURRENTS_COD_ID, nah));
}

void sendRPDOs() {
    voltage = analogIn.read() * 3.6 * 100;
    printf("voltage: %u\n", voltage);
    sendThrottleDemand();
    sendMaxCurrents();
}

void decodeTPDO(CANMessage message) {
    
}

void handleTPDO() {
    //adds the CAN message to a decoder that is added to the eventqueue
    CANMessage message;
    if (can.read(message)) {
        // Read operation successful, now call the decoding function
        TPDOqueue.call(&decodeTPDO, message);
    }
}


int main() {
    dispatcher.start(callback(&TPDOqueue, &EventQueue::dispatch_forever));
    can.attach(&handleTPDO, CAN::RxIrq);
    while(1) {
        sendRPDOs();
        wait_us(1000);
    }
}
