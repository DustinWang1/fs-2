#include "mbed.h"
#include <bitset>
#include <chrono>
#include <cstdint>

CAN *can;
EventQueue RxMsgs(4*EVENTS_EVENT_SIZE);
EventQueue TxMsgs(4*EVENTS_EVENT_SIZE);
Timer TotalRunTime;
Thread RxDispatcher;

void initIO();
void addRxToEventQueue();
void handleRx(CANMessage msg, std::chrono::microseconds timeStamp);
void RxErrorCall(std::chrono::microseconds errorTime);
void sendCANMessage();

int main() {
    //initialize
    initIO();

    //start receivin that bitc w a thread
    RxDispatcher.start(callback(&RxMsgs, &EventQueue::dispatch_forever));

    //start sendin that bitc w the main thread
    int send_id = TxMsgs.call_every(1000ms, &sendCANMessage);
    TxMsgs.dispatch_forever();

    //This one just in case the thing ends..
    while(1) {}
}

void initIO() {
    printf("init io start");
    //Start Total Run Timer
    TotalRunTime.start();
    //initialize can
    can = new CAN(PA_11, PA_12, 500000);

    //Attach ISR to receive interrupt
    can->attach(addRxToEventQueue);
    printf("Initialization Complete");
}

void addRxToEventQueue() {
    CANMessage msg;
    if(can->read(msg)) {
        RxMsgs.call(&handleRx, msg, TotalRunTime.elapsed_time());
        return;
    } else {
        RxMsgs.call(&RxErrorCall, TotalRunTime.elapsed_time());
    }
}

void handleRx(CANMessage msg, std::chrono::microseconds timeStamp) {
    unsigned int ID = msg.id;
    auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(timeStamp);
    auto remainingMicros = timeStamp - std::chrono::duration_cast<std::chrono::microseconds>(durationSeconds);
    auto durationMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(remainingMicros);
    printf("Received Message at %lld seconds %lld milliseconds:\nID: %u \nValue: %s", durationSeconds.count(), durationMilliseconds.count(), ID, msg.data);
}

void RxErrorCall(std::chrono::microseconds errorTime) {
    // Convert microseconds to seconds and milliseconds
    auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(errorTime);
    auto remainingMicros = errorTime - std::chrono::duration_cast<std::chrono::microseconds>(durationSeconds);
    auto durationMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(remainingMicros);
    // Print the duration in the desired format
    printf("Missed Rx at %lld seconds %lld milliseconds\n", durationSeconds.count(), durationMilliseconds.count());
}

void sendCANMessage() {
    char data[1];
    data[0] = 'A';

    can->write(CANMessage(1337, data, 1));

    auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(TotalRunTime.elapsed_time());
    auto remainingMicros = TotalRunTime.elapsed_time() - std::chrono::duration_cast<std::chrono::microseconds>(durationSeconds);
    auto durationMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(remainingMicros);
    printf("Sent Message at %lld seconds %lld milliseconds\n", durationSeconds.count(), durationMilliseconds.count());
}