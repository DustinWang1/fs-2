#include "mbed.h"

class CANOpenNode {
public:

    const int CAN_MESSAGE_MAX_SIZE = 11+64;

    CANOpenNode(CAN *canInterface, uint8_t nodeId);

    CANOpenNode(uint8_t nodeId);

    //All RPDO params
    const unsigned int THROTTLE_DEMAND_COB_ID = 0x186;
    const unsigned int MAX_CURRENTS_COD_ID = 0x286;

    /*
    Throttle demand params
    */
    int16_t torqueDemand = 10; //16 bit
    uint16_t maxSpeed = 10; //16 bit
    //four bit missing
    uint8_t powerReady = 1;// 1bit
    //one bit missing
    uint8_t reverseState = 0;//1bit
    uint8_t  forwardState = 1;//1bit
    //four bits missing
    uint8_t MBB_alive = 12; //4 bit
    //16 bits missing

    /*
    Max Currents params
    */
    int16_t chargeCurrentLimit;
    int16_t dischargeCurrentLimit;


    /*
    TPDO values
    */

    uint16_t speed;
    int16_t torque;
    unsigned char utilValues[2];
    
    /* utilValues contains these values ignore the types all of them are one bit
    uint8_t SOC_Low_Traction;
    uint8_t SOC_Low_Hydraulic;
    uint8_t reverse;
    uint8_t forward;
    uint8_t park_brake;
    uint8_t pedal_brake;
    uint8_t controller_Overtermp;
    uint8_t Key_switch_overvolt;
    uint8_t Key_switch_undervolt;
    uint8_t Running;
    uint8_t Traction;
    uint8_t hydraulic;
    uint8_t Powering_Enabled;
    uint8_t Powering_Ready;
    uint8_t Precharging;
    uint8_t contactor_closed;
    */
    uint16_t MotorFlags;

    uint8_t MotorTemperature;
    uint8_t ControllerTemperature;
    uint16_t DC_Bus_V;
    uint8_t FaultCode;
    uint8_t FaultLevel;
    int16_t BusCurrent;
    /*
    Utility Functions
    */
    CANMessage sendThrottleDemand();
    CANMessage sendMaxCurrents();
    void sendRPDOs(AnalogIn& HE1, AnalogIn& HE2);
    static void handleTorqueSpeed(unsigned char data[8], CANOpenNode &canHandle);
    static void handleTemperature(unsigned char data[8], CANOpenNode &canHandle);
    static void decodeTPDO(CANMessage message, CANOpenNode &canHandle);

private:
    // CAN hardware interface
    CAN *can;

    // CANopen Node ID
    uint8_t nodeId;
};

