#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to enable it
#endif

BluetoothSerial SerialBT;

#define PIN_I1  26
#define PIN_I2  27
#define PIN_I3  12
#define PIN_I4  13

const bool INVERT_LEFT_MOTOR  = false;
const bool INVERT_RIGHT_MOTOR = true; 

const bool IGNORE_RELEASE_ZERO = false;

int driveSpeed = 165;
int turnSpeed  = 135;
int spinSpeed  = 140;

const int SPEED_STEP = 5; 
const int MIN_SPEED  = 100;
const int MAX_SPEED  = 255;

const int SPIN_360_TIME_MS = 950;
const int PWM_FREQ = 500;
const int PWM_RES  = 8;

char currentCommand = 'S';

void setMotors(int leftSpeed, int rightSpeed) {
  leftSpeed  = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  if (INVERT_LEFT_MOTOR)  leftSpeed  = -leftSpeed;
  if (INVERT_RIGHT_MOTOR) rightSpeed = -rightSpeed;

  if (leftSpeed > 0) {
    analogWrite(PIN_I1, leftSpeed);
    analogWrite(PIN_I2, 0);
  } else if (leftSpeed < 0) {
    analogWrite(PIN_I1, 0);
    analogWrite(PIN_I2, -leftSpeed);
  } else {
    analogWrite(PIN_I1, 0);
    analogWrite(PIN_I2, 0);
  }

  if (rightSpeed > 0) {
    analogWrite(PIN_I3, rightSpeed);
    analogWrite(PIN_I4, 0);
  } else if (rightSpeed < 0) {
    analogWrite(PIN_I3, 0);
    analogWrite(PIN_I4, -rightSpeed);
  } else {
    analogWrite(PIN_I3, 0);
    analogWrite(PIN_I4, 0);
  }
}

void stopMotors() {
  setMotors(0, 0);
}

void turnLeft() {
  setMotors(-turnSpeed, turnSpeed); 
}

void turnRight() {
  setMotors(turnSpeed, -turnSpeed);
}

void execute360Spin() {
  setMotors(spinSpeed, -spinSpeed);
  delay(SPIN_360_TIME_MS);
  stopMotors();
  currentCommand = 'S';
}

void applyMotorState(char cmd) {
  switch (cmd) {
    case '2':
    case 'F':
      setMotors(driveSpeed, driveSpeed);
      break;

    case '8':
    case 'B':
      setMotors(-driveSpeed, -driveSpeed);
      break;

    case '4':
    case 'R':
      turnLeft();
      break;

    case '6':
    case 'L':
      turnRight();
      break;

    case '5':
    case 'S':
    case '0':
      stopMotors();
      break;

    case 'A':
    case '*':
    case 'X':
      execute360Spin();
      break;

    case '1':
      driveSpeed = 130;
      turnSpeed  = 120;
      Serial.println("Preset: SLOW (Drive: 130, Turn: 120)");
      break;

    case '3':
      driveSpeed = 165;
      turnSpeed  = 135;
      Serial.println("Preset: MEDIUM (Drive: 165, Turn: 135)");
      break;

    case '7':
      driveSpeed = 200;
      turnSpeed  = 155;
      Serial.println("Preset: FAST (Drive: 200, Turn: 155)");
      break;

    case '9':
      driveSpeed = 240;
      turnSpeed  = 175;
      Serial.println("Preset: MAX (Drive: 240, Turn: 175)");
      break;

    case '+':
      driveSpeed = constrain(driveSpeed + SPEED_STEP, MIN_SPEED, MAX_SPEED);
      turnSpeed  = constrain(turnSpeed  + SPEED_STEP, MIN_SPEED, MAX_SPEED);
      Serial.printf("All Speeds UP -> Drive: %d, Turn: %d\n", driveSpeed, turnSpeed);
      break;

    case '-':
      driveSpeed = constrain(driveSpeed - SPEED_STEP, MIN_SPEED, MAX_SPEED);
      turnSpeed  = constrain(turnSpeed  - SPEED_STEP, MIN_SPEED, MAX_SPEED);
      Serial.printf("All Speeds DOWN -> Drive: %d, Turn: %d\n", driveSpeed, turnSpeed);
      break;

    default:
      break;
  }
} 

void setup() {
  Serial.begin(115200);

  analogWriteFrequency(PIN_I1, PWM_FREQ);
  analogWriteResolution(PIN_I1, PWM_RES);
  
  analogWriteFrequency(PIN_I2, PWM_FREQ);
  analogWriteResolution(PIN_I2, PWM_RES);
  
  analogWriteFrequency(PIN_I3, PWM_FREQ);
  analogWriteResolution(PIN_I3, PWM_RES);
  
  analogWriteFrequency(PIN_I4, PWM_FREQ);
  analogWriteResolution(PIN_I4, PWM_RES);

  stopMotors();

  SerialBT.begin("ESP32_DEFENSE_ROVER");
  Serial.println("Bluetooth ready. Connect from the app!");
}

void loop() {
  while (SerialBT.available() > 0) {
    char incoming = SerialBT.read();

    if (incoming == '\n' || incoming == '\r' || incoming == ' ') {
      continue;
    }

    if (IGNORE_RELEASE_ZERO && incoming == '0') {
      continue;
    }

    incoming = toupper(incoming);

    Serial.print("Command Received: ");
    Serial.println(incoming);

    if (incoming == '2' || incoming == 'F' ||
        incoming == '8' || incoming == 'B' ||
        incoming == '4' || incoming == 'R' ||
        incoming == '6' || incoming == 'L' ||
        incoming == '5' || incoming == 'S' || incoming == '0' ||
        incoming == 'A' || incoming == '*' || incoming == 'X' ||
        incoming == '1' || incoming == '3' || incoming == '7' || incoming == '9' ||
        incoming == '+' || incoming == '-') {
      currentCommand = incoming;
      applyMotorState(currentCommand);
    }
  }

  delay(10);
}
