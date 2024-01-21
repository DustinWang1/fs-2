#include "mbed.h"

class CANOpenNode {
public:

    const int CAN_MESSAGE_MAX_SIZE = 11+64;

    CANOpenNode(CAN& canInterface, uint8_t nodeId);

    EventQueue TPDOqueue;
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
    Utility Functions
    */
    void sendThrottleDemand();
    void sendMaxCurrents();
    void sendRPDOs(AnalogIn& analogIn);
    void decodeTPDO(CANMessage message);
    void handleTPDO();

private:
    // CAN hardware interface
    CAN& can;

    // CANopen Node ID
    uint8_t nodeId;
};

