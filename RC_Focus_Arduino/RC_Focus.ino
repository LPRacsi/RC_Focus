/* This code is used for Controlling 1/24 RC focus via bluetooth
 *  Author: Rácz László
*/

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


/*commands*/
#define ACCELERATE_C  "ACC"
#define BRAKE_C       "BRA"
#define LIGHT_C       "LIG"
#define ANGLE_C       "ANG"
#define SHIFT_C       "SHI"

#define TURN_SIGNAL_BLINK_TIME 500 //us
#define CONNECTION_LOST_TIME 2000 //us

char incomingByte = "";   // for incoming serial data
String message, command;
bool commandReceived, blinkRight, blinkLeft, rTurnIsOn, lTurnIsOn, connLostHandelled;
int value, lightOnOff;
unsigned long currTime, lastRightBlinkTime, lastLeftBlinkTime, lastCommandTime;

/*
 * Function:    blinkRightTurnSignal
 * Description: 
*/

void blinkRightTurnSignal(){
  currTime = millis();
  if (currTime - lastRightBlinkTime > TURN_SIGNAL_BLINK_TIME){
    if (rTurnIsOn){
      digitalWrite(FRONT_RIGHT_TURN_PIN, LOW);
      rTurnIsOn = false;
    }else{
      digitalWrite(FRONT_RIGHT_TURN_PIN, HIGH);
      rTurnIsOn = true;
    }
    lastRightBlinkTime = millis();
  }
}


/*
 * Function:    blinkLeftTurnSignal
 * Description: 
*/

void blinkLeftTurnSignal(){
  currTime = millis();
  if (currTime - lastLeftBlinkTime > TURN_SIGNAL_BLINK_TIME){
    if (lTurnIsOn){
      digitalWrite(FRONT_LEFT_TURN_PIN, LOW);
      lTurnIsOn = false;
    }else{
      digitalWrite(FRONT_LEFT_TURN_PIN, HIGH);
      lTurnIsOn = true;
    }
    lastLeftBlinkTime = millis();
  }
}


/*
 * Function:    handleConnectionLost
 * Description: Handling of the connection lost situation.
 *              Set motor PWM to 0, Set straight steering, make a led louble blink.
*/

void handleConnectionLost(){
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
 * Function:    setLight
 * Description: Set the light pins based on the request
*/

void setLight(int onOff){
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, onOff);
  digitalWrite(FRONT_LEFT_LIGHT_PIN, onOff);  
}

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(FRONT_RIGHT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_LEFT_LIGHT_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_TURN_PIN, OUTPUT);
  pinMode(FRONT_LEFT_TURN_PIN, OUTPUT);
  digitalWrite(FRONT_RIGHT_LIGHT_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TURN_PIN, LOW); 
  digitalWrite(FRONT_LEFT_LIGHT_PIN, LOW); 
  digitalWrite(FRONT_LEFT_TURN_PIN, LOW);  
  commandReceived = false;
  blinkRight = false;
  blinkLeft = false;
  rTurnIsOn = false;
  lTurnIsOn = false;
  connLostHandelled = false;
  lastRightBlinkTime = millis();
  lastLeftBlinkTime = millis();
}

void loop() {
  if(Serial.available()){
    incomingByte = Serial.read();
    Serial.println("Fogadott: ");
    Serial.println(incomingByte);
    if (isWhitespace(incomingByte)){
      /*Do nothing here*/
    }else{
      //Serial.println(incomingByte);
      if (incomingByte == '@'){//The control sign
        command = message.substring(0,3);
        value = message.substring(3).toInt();
        message = "";
        commandReceived = true;
        connLostHandelled = false;
      }else{
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
  if (commandReceived){
    if (command == ANGLE_C){
      if (value < -20){
        blinkRight = true;
        blinkLeft = false;
      }else if(value > 20){
        blinkRight = false;
        blinkLeft = true;
      }else{
        blinkRight = false;
        blinkLeft = false;
      }
    }
    if (command == LIGHT_C){
      if (value > 0){
        lightOnOff = HIGH;
      }else if (value < 0){
        lightOnOff = LOW;
      }      
    }
  }
  if (!connLostHandelled){
    if (blinkRight || rTurnIsOn){
      blinkRightTurnSignal();
    }
    if (blinkLeft || lTurnIsOn){
      blinkLeftTurnSignal();
    }
    if (lightOnOff == HIGH || lightOnOff == LOW){
      setLight(lightOnOff);
    }
  }
  currTime = millis();
  if (currTime - lastCommandTime > CONNECTION_LOST_TIME && !connLostHandelled){
    handleConnectionLost();
    connLostHandelled = true;
  }
}
