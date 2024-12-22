// target.h
#ifndef TARGET_h
#define TARGET_h

#include <Arduino.h>
#include "DFRobotDFPlayerMini.h";

class TARGET {
  private:
    // reference to the audio
    DFRobotDFPlayerMini* myDFPlayer;

    // player vars
    bool isHit = false;
    bool isDiscoTime = false;
    uint8_t targetIndex;
    uint8_t gpioPin;
    uint8_t ledGpioPin;
    uint8_t blinkInterval = 300; // default 300ms
    unsigned long previousBlinkTime = 0; 
    int ledState = LOW;
    
  public:
    TARGET(
      uint8_t targetIndex,
      uint8_t gpioPin,
      uint8_t ledPin,
      DFRobotDFPlayerMini* dfPlayer);

    void initializeHardware();
    bool isTargetDown();
    void enableDiscoTime();

    /**
      Loop should be called as many times as you can from the main loop
      */
    void loop();
    void reset();
    int getIndex();
    int getTargetState();
};

#endif