// targetPusher.h
#ifndef TARGETPUSHER_h
#define TARGETPUSHER_h

#include <Arduino.h>
#include <Servo.h>

class TARGETPUSHER {
  private:
    // servo vars
    Servo myservo;  // create servo object to control a servo
    const int minAngle = 0;
    const int maxAngle = 230;
    int pos = maxAngle;    // variable to store the servo position

    // timer vars
    unsigned long startingMillis = 0;
    unsigned long currentMillis;
    unsigned long runMillis = 15;

    bool isPushing = false;
    bool pushingState = true; // true to reset the targets, false to retract
    uint8_t gpioPin;

  public:
    TARGETPUSHER(uint8_t gpioPin);

    void initializeHardware();
    bool handleMovement(); // this is the replacement for loop, should be called when the pusher is enabled every loop
    void enablePusher();
    bool getIsPushing();
};

#endif