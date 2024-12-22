// Include the ESP32 Arduino Servo Library instead of the original Arduino Servo Library
//#include <ESP32Servo.h>
#include <Servo.h>  // ServoESP32FIX
#include "target.h";
#include "targetPusher.h";
//#include <ezButton.h>
#include "DFRobotDFPlayerMini.h";
//#include <DFPlayerMini_Fast.h>;

DFRobotDFPlayerMini myDFPlayer;

#define RXD2 16
#define TXD2 17
#define BUSYPIN 4

unsigned long startingMillis = 0;
unsigned long victoryWaitForReset = 1000; 
bool isVictoryRetractionTriggered = false;

const int BUTTON_NUM = 5;

const int BUTTON_1_PIN = 27;
const int BUTTON_2_PIN = 26;
const int BUTTON_3_PIN = 25;
const int BUTTON_4_PIN = 32;
const int BUTTON_5_PIN = 33;

int buttonPinArray[] = {
	BUTTON_1_PIN, 
	BUTTON_2_PIN, 
	BUTTON_3_PIN, 
	BUTTON_4_PIN, 
	BUTTON_5_PIN
};

TARGET *targetArray[] = {
  new TARGET(1, BUTTON_1_PIN, 23, &myDFPlayer),
  new TARGET(2, BUTTON_2_PIN, 13, &myDFPlayer),
  new TARGET(3, BUTTON_3_PIN, 22, &myDFPlayer),
  new TARGET(4, BUTTON_4_PIN, 19, &myDFPlayer),
  new TARGET(5, BUTTON_5_PIN, 21, &myDFPlayer),
};

TARGETPUSHER *targetPusher = new TARGETPUSHER(18);

// The game state enum
// 0 = init
// 1 = resetting targets
// 2 = waiting for targets to be shot down
// 3 = victory, resetting the targets
int current_game_state = 0;

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //while (!Serial.available()) {}

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);

  targetPusher->initializeHardware();

  pinMode(BUSYPIN, INPUT);

  // initliaze the audio
  setupAudio();

  // initialize the hardware for all the targets
  for(byte i = 0; i < BUTTON_NUM; i++){
    targetArray[i]->initializeHardware();
	}

  // wait for servo
  delay(1000);
}

void setupAudio() {
  // Try to initialize the SD cardl
  if (!myDFPlayer.begin(Serial2, true, false))
  {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }

  // default audio settings
  myDFPlayer.setTimeOut(500); // Timeout serial 500ms
  myDFPlayer.volume(25); // Volume 0 - 30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

  // Play the welcome audio
  myDFPlayer.playMp3Folder(random(1, 3)); //play specific mp3 in SD:/MP3/0002.mp3; File Name(0~65535)
}

void loop() {

  // handle the initialization
  if (current_game_state == 0) {
    handleGameInitialization();
  }
  else if (current_game_state == 1) {
    handleTargetPreperation();
  }
  else if (current_game_state == 2) {
    handleTargetShooting();
  }
  else if (current_game_state == 3) {
    handleVictory();
  } 
}

void handleGameInitialization() {
  // check if any target is still down
  // if so, we need to reset first
  bool isAnyTargetDown = false;
  for(byte i = 0; i < BUTTON_NUM; i++)
    if (targetArray[i]->getTargetState() == LOW)
      isAnyTargetDown = true;

  if (isAnyTargetDown) {
    targetPusher->enablePusher();
    // switch to the second game loop
    current_game_state = 1;
  } else {
    // we can continue to the gameplay loop
    current_game_state = 2;
  }
}

void handleTargetPreperation() {
  bool result = targetPusher->handleMovement();
  // wait until it is finished pushing and retracting
  if (!result) {
    // reset the targets
    for(byte i = 0; i < BUTTON_NUM; i++)
      targetArray[i]->reset();

    // move to playing
    current_game_state = 2;
  }
}

void handleTargetShooting() {
  // loop all the targets
  for(byte i = 0; i < BUTTON_NUM; i++)
    targetArray[i]->loop(); 

  // then we check if all the targets are shot down
  bool allTargetsDown = true;
  for(byte i = 0; i < BUTTON_NUM; i++)
  {
    if (targetArray[i]->isTargetDown() == false)
      allTargetsDown = false;
  }

  if (allTargetsDown) {
    delay(2000);

    // enable the disco lights ;)
    for(byte i = 0; i < BUTTON_NUM; i++)
      targetArray[i]->enableDiscoTime();

    // victory music here!
    myDFPlayer.playMp3Folder(random(10, 16)); //play specific mp3 in SD:/MP3/0002.mp3; File Name(0~65535)
    delay(1000); // give time to actually play the mp3
    isVictoryRetractionTriggered = false;

    // move to the victory state
    current_game_state = 3;
  }
}

void handleVictory() {

  if (!isVictoryRetractionTriggered || !targetPusher->getIsPushing()) {
    bool donePlayingMp3 = false;
    uint32_t timeChecked = millis();
    while (!donePlayingMp3) {
      int busyState = digitalRead(BUSYPIN);
      // delay in checks
      if (millis() - timeChecked > 200) {
        timeChecked = millis();
        if (busyState == HIGH) 
          donePlayingMp3 = true;
      }

      // update the led's
      for(byte i = 0; i < BUTTON_NUM; i++)
        targetArray[i]->loop(); 
    }

    // reset the targets
    for(byte i = 0; i < BUTTON_NUM; i++)
      targetArray[i]->reset();
  
    targetPusher->enablePusher();
    isVictoryRetractionTriggered = true;

  } else {
    bool result = targetPusher->handleMovement();
    // wait until it is finished pushing and retracting
    if (!result) {
      // reset the targets
      for(byte i = 0; i < BUTTON_NUM; i++)
        targetArray[i]->reset();

      // return to playing
      isVictoryRetractionTriggered = false;
      current_game_state = 2;
    }
  }
}