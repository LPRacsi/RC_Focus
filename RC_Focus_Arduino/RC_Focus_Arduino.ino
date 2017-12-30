/* This code is used for Controlling 1/24 RC focus via bluetooth
    Author: Rácz László
*/

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>


SoftwareSerial mySoftwareSerial(3, 2); // RX, TX
Servo turnServo;

DFRobotDFPlayerMini myDFPlayer;


/*Pin definitions*/
#define FRONT_RIGHT_LIGHT_PIN  9
#define FRONT_LEFT_LIGHT_PIN   10
#define FRONT_RIGHT_TURN_PIN   12
#define FRONT_LEFT_TURN_PIN    11
/*#define REAR_RIGHT_LIGHT_PIN   9
  #define REAR_LEFT_LIGHT_PIN    5
  #define REAR_RIGHT_TURN_PIN   5
  #define REAR_LEFT_TURN_PIN   5
  #define BRAKE_RIGHT_LIGHT_PIN   5
  #define BRAKE_LEFT_LIGHT_PIN   5
  #define REVERSE_LIGHT_PIN   5*/
#define ACCELERATE_PWM_PIN_1 5
#define ACCELERATE_PWM_PIN_2 6
#define TURN_ENABLE_PIN      4
#define TURN_SERVO_PIN       7

/*commands*/
#define ACCELERATE_C  "ACC"
#define BRAKE_C       "BRA"
#define LIGHT_C       "LIG"
#define ANGLE_C       "ANG"
#define SHIFT_C       "SHI"
#define MUSIC_C       "MUS"

#define PLAY_C    1
#define STOP_C    2
#define NEXT_C    3
#define PREV_C    4
#define VOLUP_C   5
#define VOLDOWN_C 6


#define TURN_SIGNAL_BLINK_TIME 500 //us
#define CONNECTION_LOST_TIME 2000 //us

char incomingByte = "";   // for incoming serial data
String message, command;
bool commandReceived, blinkRight, blinkLeft, rTurnIsOn, lTurnIsOn, connLostHandelled;
int value, lightOnOff, servoAngle;
unsigned long currTime, lastRightBlinkTime, lastLeftBlinkTime, lastCommandTime;
//int command2;
/*
   Function:    blinkRightTurnSignal
   Description:
*/

void blinkRightTurnSignal() {
  currTime = millis();
  if (currTime - lastRightBlinkTime > TURN_SIGNAL_BLINK_TIME) {
    if (rTurnIsOn) {
      digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
      rTurnIsOn = false;
    } else {
      digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
      rTurnIsOn = true;
    }
    lastRightBlinkTime = millis();
  }
}


/*
   Function:    blinkLeftTurnSignal
   Description:
*/

void blinkLeftTurnSignal() {
  currTime = millis();
  if (currTime - lastLeftBlinkTime > TURN_SIGNAL_BLINK_TIME) {
    if (lTurnIsOn) {
      digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
      lTurnIsOn = false;
    } else {
      digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
      lTurnIsOn = true;
    }
    lastLeftBlinkTime = millis();
  }
}


/*
   Function:    handleConnectionLost
   Description: Handling of the connection lost situation.
                Set motor PWM to 0, Set straight steering, make a led louble blink.
*/

void handleConnectionLost() {
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
  delay(200);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
}


/*
   Function:    setLight
   Description: Set the light pins based on the request
*/

void setLight(int onOff) {
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, onOff);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, onOff);
}

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  pinMode(FRONT_RIGHT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_LEFT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_TURN_PIN, OUTPUT);
  pinMode(FRONT_LEFT_TURN_PIN, OUTPUT);
  pinMode(TURN_ENABLE_PIN, OUTPUT);
  pinMode(ACCELERATE_PWM_PIN_1, OUTPUT);
  pinMode(ACCELERATE_PWM_PIN_2, OUTPUT);
  commandReceived = false;
  blinkRight = false;
  blinkLeft = false;
  rTurnIsOn = false;
  lTurnIsOn = false;
  connLostHandelled = false;
  servoAngle = 88;
  lastRightBlinkTime = millis();
  lastLeftBlinkTime = millis();
  myDFPlayer.play(random(0, 22));
  turnServo.attach(TURN_SERVO_PIN);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  turnServo.write(servoAngle); // This should be the straight
  digitalWrite(ACCELERATE_PWM_PIN_1, HIGH);
  digitalWrite(ACCELERATE_PWM_PIN_2, LOW);
  digitalWrite(TURN_ENABLE_PIN, HIGH); // Enable the power supply of the servo
}

void loop() {
 
  /*if (Serial.available()) {
    command = Serial.readString();
    command2 = command.toInt();
    servoAngle = servoAngle + command2;
    Serial.print("Angle: ");
    Serial.println(servoAngle);
    turnServo.write(servoAngle);
  }*/
  /*for (int i = 80; i < 100; i++) {
    servoAngle++;
    turnServo.write(servoAngle);
    delay(1000);
    Serial.print("Angle: ");
    Serial.println(servoAngle);
  }
  for (int j = 100; j > 80; j--) {
    servoAngle--;
    turnServo.write(servoAngle);
    delay(1000);
    Serial.print("Angle: ");
    Serial.println(servoAngle);
  }*/
  delay(1000);
  turnServo.write(90);
  Serial.println("90 fok ");
  delay(1000);
  turnServo.write(80);
  Serial.println("80 fok");
  if (Serial.available()) {
    incomingByte = Serial.read();
    Serial.println("Fogadott: ");
    Serial.println(incomingByte);
    if (isWhitespace(incomingByte)) {
      /*Do nothing here*/
    } else {
      //Serial.println(incomingByte);
      if (incomingByte == '@') { //The control sign
        command = message.substring(0, 3);
        value = message.substring(3).toInt();
        message = "";
        commandReceived = true;
        connLostHandelled = false;
      } else {
        commandReceived = false;
        message += incomingByte;
        lastCommandTime = millis();
      }
      Serial.println("command: ");
      Serial.println(command);
      Serial.println("value: ");
      Serial.println(value);
    }
  }
  if (commandReceived) {
    if (command == ANGLE_C) {
      if (value < -20) {
        blinkRight = true;
        blinkLeft = false;
      } else if (value > 20) {
        blinkRight = false;
        blinkLeft = true;
      } else {
        blinkRight = false;
        blinkLeft = false;
      }
    }
    if (command == LIGHT_C) {
      if (value > 0) {
        lightOnOff = HIGH;
      } else if (value < 0) {
        lightOnOff = LOW;
      }
    }
    if (command == MUSIC_C) {
      switch (value) {
        case PLAY_C:    myDFPlayer.play(random(0, 22));
          break;
        case STOP_C:    myDFPlayer.pause();
          break;
        case NEXT_C:    myDFPlayer.next();
          break;
        case PREV_C:    myDFPlayer.previous();
          break;
        case VOLUP_C:   myDFPlayer.volumeUp();
          break;
        case VOLDOWN_C: myDFPlayer.volumeDown();
          break;
        default:        myDFPlayer.reset();
          break;
      }
    }
    command = "";
    value = 0;
  }
  if (!connLostHandelled) {
    if (blinkRight || rTurnIsOn) {
      blinkRightTurnSignal();
    }
    if (blinkLeft || lTurnIsOn) {
      blinkLeftTurnSignal();
    }
    if (lightOnOff == HIGH || lightOnOff == LOW) {
      setLight(lightOnOff);
    }
  }
  currTime = millis();
  if (currTime - lastCommandTime > CONNECTION_LOST_TIME && !connLostHandelled) {
    handleConnectionLost();
    connLostHandelled = true;
  }
}
