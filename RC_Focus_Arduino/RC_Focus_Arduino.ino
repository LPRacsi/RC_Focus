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
#define FRONT_RIGHT_LIGHT_PIN  12
#define FRONT_LEFT_LIGHT_PIN   11
#define FRONT_RIGHT_TURN_PIN   10
#define FRONT_LEFT_TURN_PIN    9
#define REAR_RIGHT_LIGHT_PIN   13
#define REAR_LEFT_LIGHT_PIN    A0
#define REAR_RIGHT_TURN_PIN    A1
#define REAR_LEFT_TURN_PIN     A2
#define BRAKE_RIGHT_LIGHT_PIN  A3
#define BRAKE_LEFT_LIGHT_PIN   A4
#define ACCELERATE_PWM_PIN_1   5
#define ACCELERATE_PWM_PIN_2   6
#define TURN_ENABLE_PIN        4
#define TURN_SERVO_PIN         7
#define VOLT_MEAS_PIN          A5

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
#define CONNECTION_LOST_TIME 10000 //us

char incomingByte = "";   // for incoming serial data
String message, command;
bool commandReceived, blinkRight, blinkLeft, rTurnIsOn, lTurnIsOn, connLostHandelled, acceleratePressed;
short value, lightOnOff, servoAngle, gear, targetPWM, actualPWM;
unsigned long currTime, lastRightBlinkTime, lastLeftBlinkTime, lastCommandTime, prevSpeedUpdateTime;
unsigned short gearPWM[] = {80, 0, -80, -100, -120, -140, -160};
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
 * Function:    getBatteryVoltage
 * Description: Reads the measured voltage and calculates the battery voltage
 * Return:      The input battery voltage
*/
float getBatteryVoltage(){
  float vIn = 0.0; 
  float vOut = 0.0; 
  float R1 = 10000.0;
  float R2 = 1000.0;
  int readBatt = 0;
  readBatt = analogRead(VOLT_MEAS_PIN);
  //Serial.print("Measured readBatt: ");
  //Serial.println(readBatt);  
  vOut = (readBatt * 5.0) / 1024.0; 
  vIn = vOut / ( R2 / (R1 + R2)); 
  if (vIn < 0.09) {
    vIn = 0.0;//statement to quash undesired reading !
  }
  return vIn;
}
/*
 * Function:    setAllLEDOn
 * Desctiption: Set all LED on
 */
 void setAllLEDOn(){
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_LEFT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_RIGHT_TURN_PIN, HIGH);
  digitalWrite(REAR_LEFT_TURN_PIN, HIGH);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, HIGH); 
 }
/*
   Function:    handleConnectionLost
   Description: Handling of the connection lost situation.
                Set motor PWM to 0, Set straight steering, make a led louble blink.
*/

void handleConnectionLost() {
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(REAR_LEFT_LIGHT_PIN, LOW);
  digitalWrite(REAR_RIGHT_TURN_PIN, LOW);
  digitalWrite(REAR_LEFT_TURN_PIN, LOW);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, LOW); 
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_LEFT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_RIGHT_TURN_PIN, HIGH);
  digitalWrite(REAR_LEFT_TURN_PIN, HIGH);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, HIGH);   
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(REAR_LEFT_LIGHT_PIN, LOW);
  digitalWrite(REAR_RIGHT_TURN_PIN, LOW);
  digitalWrite(REAR_LEFT_TURN_PIN, LOW);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, LOW); 
  delay(100);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_LEFT_LIGHT_PIN, HIGH);
  digitalWrite(REAR_RIGHT_TURN_PIN, HIGH);
  digitalWrite(REAR_LEFT_TURN_PIN, HIGH);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, HIGH);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, HIGH);   
  delay(200);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(REAR_LEFT_LIGHT_PIN, LOW);
  digitalWrite(REAR_RIGHT_TURN_PIN, LOW);
  digitalWrite(REAR_LEFT_TURN_PIN, LOW);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, LOW);
  servoAngle = 82;
  turnServo.write(servoAngle);
  cotrolSpeed(0);
  actualPWM = 0;
  targetPWM = 0;
}


/*
   Function:    setLight
   Description: Set the light pins based on the request
*/

void setLight(int onOff) {
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, onOff);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, onOff);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, onOff);
  digitalWrite(REAR_LEFT_LIGHT_PIN, onOff);  
}
/*
 * Function:  
 * Description: 
 */
int inputAngleToServoAngle(int inputAngle){
  if (inputAngle >= 90){
    inputAngle = 90;
  }else if (inputAngle <= -90){
    inputAngle = -90;
  }
  return map(inputAngle, -90, 90, 74, 90);
}

/*
 * Function:    CotrolSpeed
 * Parameter:   speedPWM: PWM and direction for the motor
 * Description: Handlethe speed and direction of the motor
*/
void cotrolSpeed(int speedPWM){
  
  if(speedPWM >= 0){
    analogWrite(ACCELERATE_PWM_PIN_1, (speedPWM));   // set speed for the motor
    analogWrite(ACCELERATE_PWM_PIN_2, LOW);        
  }
  else if(speedPWM < 0){
    speedPWM = abs(speedPWM);
    analogWrite(ACCELERATE_PWM_PIN_1, LOW);   // set speed for the motor
    analogWrite(ACCELERATE_PWM_PIN_2, (speedPWM));     
  }
}

/*
 * Function:   controlVehicleSpeed
 * Parameter:  toPWM: the target PWM limit
 *             fromPWM: the actual PWM
 * Description:Control the vehicle speed based on the input gear
 */

void controlVehicleSpeed(short toPWM, short fromPWM){
  currTime = millis();
  if (currTime - prevSpeedUpdateTime > 250){
  /*Serial.print("fromPWM: ");
  Serial.println(fromPWM);
  Serial.print("toPWM: ");
  Serial.println(toPWM);*/  
    if (toPWM > fromPWM){
      fromPWM = fromPWM + 10;
    }else if (toPWM < fromPWM){
      fromPWM = fromPWM - 20;
    }
    actualPWM = fromPWM;
    cotrolSpeed(actualPWM);
    /*Serial.print("actualPWM: ");
    Serial.println(actualPWM);*/
    prevSpeedUpdateTime = millis();
  }
}

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  myDFPlayer.volume(25);  //Set volume value. From 0 to 30
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  pinMode(FRONT_RIGHT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_LEFT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_TURN_PIN, OUTPUT);
  pinMode(FRONT_LEFT_TURN_PIN, OUTPUT);
  pinMode(REAR_RIGHT_LIGHT_PIN, OUTPUT);
  pinMode(REAR_LEFT_LIGHT_PIN, OUTPUT);
  pinMode(REAR_RIGHT_TURN_PIN, OUTPUT);
  pinMode(REAR_LEFT_TURN_PIN, OUTPUT);
  pinMode(BRAKE_RIGHT_LIGHT_PIN, OUTPUT);
  pinMode(BRAKE_LEFT_LIGHT_PIN, OUTPUT);  
  pinMode(TURN_ENABLE_PIN, OUTPUT);
  pinMode(ACCELERATE_PWM_PIN_1, OUTPUT);
  pinMode(ACCELERATE_PWM_PIN_2, OUTPUT);
  pinMode(VOLT_MEAS_PIN,INPUT);
  commandReceived = false;
  blinkRight = false;
  blinkLeft = false;
  rTurnIsOn = false;
  lTurnIsOn = false;
  connLostHandelled = false;
  acceleratePressed = false;
  gear = 0;
  targetPWM = 0;
  actualPWM = 0;
  servoAngle = 82;
  lastRightBlinkTime = millis();
  lastLeftBlinkTime = millis();
  prevSpeedUpdateTime = millis();
  myDFPlayer.play(random(0, 22));
  turnServo.attach(TURN_SERVO_PIN);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
  digitalWrite(REAR_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(REAR_LEFT_LIGHT_PIN, LOW);
  digitalWrite(REAR_RIGHT_TURN_PIN, LOW);
  digitalWrite(REAR_LEFT_TURN_PIN, LOW);
  digitalWrite(BRAKE_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(BRAKE_LEFT_LIGHT_PIN, LOW);    
  turnServo.write(servoAngle); // This should be the straight
  digitalWrite(ACCELERATE_PWM_PIN_1, LOW);
  digitalWrite(ACCELERATE_PWM_PIN_2, LOW);
  digitalWrite(TURN_ENABLE_PIN, HIGH); // Enable the power supply of the servo
  setAllLEDOn();
  cotrolSpeed(0);
}

void loop() {
  if (Serial.available()) {
    incomingByte = Serial.read();
    if (isWhitespace(incomingByte)) {
      /*Do nothing here*/
    } else {
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
      servoAngle = inputAngleToServoAngle(value);
      turnServo.write(servoAngle);
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
    if (command == SHIFT_C){
      gear = value;
      targetPWM = gearPWM[gear+1];
    }
    if (command == BRAKE_C){
      if (value == 1){
        digitalWrite(BRAKE_RIGHT_LIGHT_PIN, HIGH);
        digitalWrite(BRAKE_LEFT_LIGHT_PIN, HIGH);        
        cotrolSpeed(0);
      }else if (value == -1){
        digitalWrite(BRAKE_RIGHT_LIGHT_PIN, LOW);
        digitalWrite(BRAKE_LEFT_LIGHT_PIN, LOW);          
      }
    }
    if (command == ACCELERATE_C){
      if (value == 1){
        acceleratePressed = true; 
      }else if (value == -1){
        acceleratePressed = false;
      }
    }
    command = "";
    value = 0;
  }
      // This is just a temp.
    //targetPWM = -130;
      // This is just a temp.
    //acceleratePressed = true;
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
    if (acceleratePressed){
      controlVehicleSpeed(targetPWM, actualPWM);
    }else {
      targetPWM = 0;
      controlVehicleSpeed(targetPWM, actualPWM);
    }
  }
  currTime = millis();
  if (currTime - lastCommandTime > CONNECTION_LOST_TIME && !connLostHandelled) {
    handleConnectionLost();
    connLostHandelled = true;
  }
}
