#include <Wire.h>
#include <stdint.h>

// I2C ///////////////////////

#define I2C_SDA 32
#define I2C_SCL 33
#define I2C_SLAVE_ADDRESS  0x11

struct {
  int16_t rightSpeed;
  int16_t leftSpeed;
} motorSpeed;

union {
  uint8_t raw[2];
  int16_t value;
} vel_l, vel_r;

uint8_t motorMode = 0x00;  // Default to nothing
unsigned long lastI2CReceiveTime = 0;
const unsigned long TIMEOUT_MS = 500;

////////////////////// End I2C

// Motors //////////////////////

const uint16_t PWMA = 25;
const uint16_t AIN2 = 17;
const uint16_t AIN1 = 21;
const uint16_t BIN1 = 22;
const uint16_t BIN2 = 23;
const uint16_t PWMB = 26;

const uint16_t ANALOG_WRITE_BITS = 8;

int freq = 10000;
int channel_A = 0;
int channel_B = 1;
int resolution = ANALOG_WRITE_BITS;

void initMotors(){
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  ledcSetup(channel_A, freq, resolution);
  ledcAttachPin(PWMA, channel_A);

  ledcSetup(channel_B, freq, resolution);
  ledcAttachPin(PWMB, channel_B);
}

void motorL(int16_t value){
  uint16_t pwm;
  if(value < 0){
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    pwm = static_cast<uint16_t>(-value);
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    pwm = static_cast<uint16_t>(value);
  }
  ledcWrite(channel_A, pwm);
}

void motorR(int16_t value){
  uint16_t pwm;
  if(value < 0){
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    pwm = static_cast<uint16_t>(-value);
  } else {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    pwm = static_cast<uint16_t>(value);
  }
  ledcWrite(channel_B, pwm);
}

void moveF(int16_t vel_l, int16_t vel_r){
  motorL(vel_l);
  motorR(vel_r);
}

void stopMotors(){
  ledcWrite(channel_A, 0);
  ledcWrite(channel_B, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void setup() {
  initMotors();
  // Serial.begin(9600);

  Wire.begin(I2C_SLAVE_ADDRESS, I2C_SDA, I2C_SCL, 0);
  Wire.onReceive(receiveData);

  stopMotors();
}

void loop() {
  unsigned long now = millis();

  if (motorMode == 0x44) {
    moveF(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
  }
  
  if (motorMode == 0x55) {
    if (now - lastI2CReceiveTime < TIMEOUT_MS) {
      moveF(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
    } else {
      stopMotors();
      motorMode == 0x00; // Move to nothing
    }
  }

  delay(10); // smoother control loop
}

// callback received data
void receiveData(int byteCount) {
  if (byteCount < 5) return;

  uint8_t discard = Wire.read(); // register byte from master

  motorMode = Wire.read(); // mode: 0x44 or 0x55

  vel_l.raw[1] = Wire.read();
  vel_l.raw[0] = Wire.read();

  vel_r.raw[1] = Wire.read();
  vel_r.raw[0] = Wire.read();

  motorSpeed.leftSpeed = vel_l.value;
  motorSpeed.rightSpeed = vel_r.value;

  lastI2CReceiveTime = millis(); // update last receive time

  // Serial.print("Mode: 0x");
  // Serial.println(motorMode, HEX);
  // Serial.print("Left Speed: ");
  // Serial.print(motorSpeed.leftSpeed);
  // Serial.print("\tRight Speed: ");
  // Serial.println(motorSpeed.rightSpeed);
}
