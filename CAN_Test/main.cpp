#include <cstdio>
#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#include "mbed.h"


AnalogIn ain(A0);

CAN can1(PA_11, PA_12, 500000); // creates the CAN bus with pins RX and TX respectively, frequency of 500000 which is the same as the inverter
char voltage = 0;

bool board1 = false; // true if sending board, false if recieving board

EventQueue queue(4*EVENTS_EVENT_SIZE); // creates an eventqueue which is thread and ISR safe. EVENTS_EVENT_SIZE is the size of the buffar allocated

void send()
{
    voltage = ain.read() * 100;
    
    if (can1.write(CANMessage(1337, &voltage, 1))) { // writes the can message with the id 1337, data voltage, and data length 1
        printf("Message sent: %d\n", voltage);
    }
}

int main()
{
    printf("main()\n");
    if (board1) {
        int send_id = queue.call_every(100ms, &send); // call send 10 times per second
        queue.dispatch_forever(); // call send forever
    } else {
        CANMessage msg; // stores the message to be read
        while (1) {
            if (can1.read(msg)) { // reads the message -- returns 0 if no message arrived
                printf("Message received, voltage: %d\n", msg.data[0]);
            }
        }
    }
}