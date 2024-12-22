// targetPusher.cpp
#include "targetPusher.h"

TARGETPUSHER::TARGETPUSHER(uint8_t gpioPin) 
{
  // set our basic target vars
  this->gpioPin = gpioPin;
}

bool TARGETPUSHER::getIsPushing() { return this->isPushing; }

void TARGETPUSHER::initializeHardware() {
  //myservo.attach(18, 0, 5500);  // attaches the servo on GPIO 18 to the servo object
  myservo.attach(
    this->gpioPin,
    Servo::CHANNEL_NOT_ATTACHED, 
    0, 
    270, 
    Servo::DEFAULT_MIN_PULSE_WIDTH_US, 
    Servo::DEFAULT_MAX_PULSE_WIDTH_US, 
    200);

  // reset the servo to its default position
  myservo.write(pos);
}

bool TARGETPUSHER::handleMovement() {
  if (!this->isPushing) return false;
  this->currentMillis = millis();

  if (this->currentMillis - this->startingMillis >= this->runMillis) {
    // determine the direction
    pos += this->pushingState ? -1 : 1;
    if (this->pushingState && pos <= minAngle) {
      this->pushingState = false; // return to retracting
      pos = minAngle; // just to be safe
    } else if (!this->pushingState && pos >= maxAngle) {
      this->pushingState = true;
      pos = maxAngle;
      this->isPushing = false; // we are done
    }
    // move the servo
    myservo.write(pos); 

    // update the timer
    this->startingMillis = this->currentMillis;
  }
  return this->isPushing;
}

void TARGETPUSHER::enablePusher() {
  this->isPushing = true;
  this->startingMillis = millis();
}