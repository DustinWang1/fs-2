#include "CO.h"
#include "mbed.h"
#include <cstdint>


CANOpenNode::CANOpenNode(CAN& canInterface, uint8_t nodeId) : can(canInterface), nodeId(nodeId), TPDOqueue(64+11) {
    canInterface.attach(callback(this, &CANOpenNode::handleTPDO), CAN::RxIrq);
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

void CANOpenNode::decodeTPDO(CANMessage message) {
    //check id
    //switch between RPDOS
    //decode for each one
}

void CANOpenNode::handleTPDO() {
    // Read the CAN message
    CANMessage msg;
    if (can.read(msg)) {
        // Example: Print the received message
        printf("Received CAN message ID: 0x%X, Length: %d, Data: ", msg.id, msg.len);
        for (int i = 0; i < msg.len; i++) {
            printf("%02X ", msg.data[i]);
        }
        printf("\n");

        TPDOqueue.call(&CANOpenNode::decodeTPDO, msg);
    }
}


