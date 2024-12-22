// target.cpp
#include "target.h"

TARGET::TARGET(
  uint8_t targetIndex,
  uint8_t gpioPin,
  uint8_t ledPin,
  DFRobotDFPlayerMini* dfPlayer) 
{
  // set our basic target vars
  this->targetIndex = targetIndex;
  this->myDFPlayer = dfPlayer;
  this->gpioPin = gpioPin;
  this->ledGpioPin = ledPin;
  this->isHit = false;
}

void TARGET::initializeHardware() {
  pinMode(this->gpioPin, INPUT_PULLUP);
  pinMode(this->ledGpioPin, OUTPUT);
}

int TARGET::getTargetState() {
  return digitalRead(this->gpioPin);
}

bool TARGET::isTargetDown() {
  return this->isHit;
}

int TARGET::getIndex() { return targetIndex; }

void TARGET::reset() {
  // reset the hit marks
  this->isHit = false;
  this->ledState = LOW;
  this->isDiscoTime = false;

  digitalWrite(this->ledGpioPin, this->ledState);
}

void TARGET::enableDiscoTime() {
  this->isDiscoTime = true;
  this->blinkInterval = random(200, 600); // random between the 200ms and 600ms
}

void TARGET::loop() {
  // check if the target is hit here, this can only happen once
  if (digitalRead(this->gpioPin) == LOW && !this->isHit && !this->isDiscoTime)
  {
    // mark it as hit
    this->isHit = true;
    this->ledState = HIGH;

    digitalWrite(this->ledGpioPin, this->ledState);

    // play a sound effect ;)
    // 100 - 103
    myDFPlayer->playMp3Folder(random(20, 27)); //play specific mp3 in SD:/MP3/0002.mp3; File Name(0~65535)
  }

  // Disco time
  if (this->isDiscoTime) {
    unsigned long currentMillis = millis();

    if (currentMillis - this->previousBlinkTime >= this->blinkInterval) {
      this->blinkInterval = random(200, 600); // random between the 200ms and 600ms
      this->previousBlinkTime = currentMillis;
      
      this->ledState = this->ledState == LOW ? HIGH : LOW;
      digitalWrite(this->ledGpioPin, this->ledState);
    }
  }
}