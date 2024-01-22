#include "CO.h"
#include "mbed.h"
#include <cstdint>


CANOpenNode::CANOpenNode(CAN& canInterface, uint8_t nodeId) : can(canInterface), nodeId(nodeId) {
 
}

void CANOpenNode::sendThrottleDemand() {
    char nah[6];
    nah[0] = (torqueDemand & 0xFF00) >> 8;
    nah[1] = (torqueDemand & 0x00FF);
    nah[2] = (maxSpeed & 0xFF00) >> 8;
    nah[3] = (maxSpeed & 0x00FF);
    nah[4] = ((powerReady | 0x00) << 3) | ((reverseState | 0x00) << 1) | (forwardState | 0x00);
    nah[5] = MBB_alive;

    can.write(CANMessage(THROTTLE_DEMAND_COB_ID, nah));
}

void CANOpenNode::sendMaxCurrents() {
    //same method as the last one
    char nah[4];
    nah[0] = (chargeCurrentLimit & 0xFF00) >> 8;
    nah[1] = (chargeCurrentLimit & 0x00FF);
    nah[2] = (dischargeCurrentLimit & 0xFF00) >> 8;
    nah[3] = (dischargeCurrentLimit & 0x00FF);

    can.write(CANMessage(MAX_CURRENTS_COD_ID, nah));
}

void CANOpenNode::sendRPDOs(AnalogIn& analogIn) {
    unsigned int voltage = analogIn.read() * 3.6 * 100;
    printf("voltage: %u\n", voltage);
    sendThrottleDemand();
    sendMaxCurrents();
}

void CANOpenNode::decodeTPDO(CANMessage message, CANOpenNode &canHandle) {
    //check id
    switch(message.id) {
        case 0x481:
            handleTorqueSpeed(message.data, canHandle);
            break;
        case 0x681:
            handleTemperature(message.data, canHandle);
            break;
    }
    //switch between RPDOS
    //decode for each one
}



void CANOpenNode::handleTorqueSpeed(unsigned char data[8], CANOpenNode &canHandle) {
    canHandle.speed = static_cast<uint16_t>(static_cast<uint8_t>(data[0])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(data[1])) << 8);
    canHandle.torque = static_cast<uint16_t>(static_cast<uint8_t>(data[2])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(data[3])) << 8);
    canHandle.utilValues[0] = data[4];
    canHandle.utilValues[1] = data[5];
    canHandle.MotorFlags =  canHandle.torque = static_cast<uint16_t>(static_cast<uint8_t>(data[6])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(data[7])) << 8);
}

void CANOpenNode::handleTemperature(unsigned char data[8], CANOpenNode &canHandle) {
    canHandle.MotorTemperature = static_cast<uint8_t>(data[0]);
    canHandle.ControllerTemperature = static_cast<uint8_t>(data[1]);
    canHandle.DC_Bus_V = static_cast<uint16_t>(static_cast<uint8_t>(data[2])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(data[3])) << 8);
    canHandle.FaultCode = static_cast<uint8_t>(data[4]);
    canHandle.FaultLevel = static_cast<uint8_t>(data[5]);
    canHandle.BusCurrent = static_cast<uint16_t>(static_cast<uint8_t>(data[6])) |
                     (static_cast<uint16_t>(static_cast<uint8_t>(data[7])) << 8);
}

