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

// Encoders //////////////////////
const uint16_t AENCA = 35;
const uint16_t AENCB = 34;
const uint16_t BENCB = 16;
const uint16_t BENCA = 27;

volatile long B_wheel_pulse_count = 0;
volatile long A_wheel_pulse_count = 0;

int interval = 100;
unsigned long lastRPMTime = 0;

double rpm_A = 0;
double rpm_B = 0;

double reduction_ratio = 42;
int ppr_num = 11;
double shaft_ppr = reduction_ratio * ppr_num;

// === Motor Functions ===
void initMotors() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);

  ledcSetup(channel_A, freq, resolution);
  ledcAttachPin(PWMA, channel_A);

  ledcSetup(channel_B, freq, resolution);
  ledcAttachPin(PWMB, channel_B);
}

void motorL(int16_t value) {
  uint16_t pwm = abs(value);
  digitalWrite(AIN1, value < 0 ? LOW : HIGH);
  digitalWrite(AIN2, value < 0 ? HIGH : LOW);
  ledcWrite(channel_A, pwm);
}

void motorR(int16_t value) {
  uint16_t pwm = abs(value);
  digitalWrite(BIN1, value < 0 ? LOW : HIGH);
  digitalWrite(BIN2, value < 0 ? HIGH : LOW);
  ledcWrite(channel_B, pwm);
}

void moveF(int16_t vel_l, int16_t vel_r) {
  motorL(vel_l);
  motorR(vel_r);
}

void stopMotors() {
  ledcWrite(channel_A, 0);
  ledcWrite(channel_B, 0);
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
}

// === Encoder Functions ===
void IRAM_ATTR B_wheel_pulse() {
  if (digitalRead(BENCA)) B_wheel_pulse_count--;
  else B_wheel_pulse_count++;
}

void IRAM_ATTR A_wheel_pulse() {
  if (digitalRead(AENCA)) A_wheel_pulse_count--;
  else A_wheel_pulse_count++;
}

void initEncoders() {
  pinMode(BENCB , INPUT_PULLUP);
  pinMode(BENCA , INPUT_PULLUP);
  pinMode(AENCB , INPUT_PULLUP);
  pinMode(AENCA , INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BENCB), B_wheel_pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(AENCB), A_wheel_pulse, RISING);
}

// === I2C Callbacks ===
void onI2CRequest() {
  int16_t rpmA_scaled = static_cast<int16_t>(rpm_A * 100.0);
  int16_t rpmB_scaled = static_cast<int16_t>(rpm_B * 100.0);

  uint8_t buffer[4];
  buffer[0] = (rpmA_scaled >> 8) & 0xFF;
  buffer[1] = rpmA_scaled & 0xFF;
  buffer[2] = (rpmB_scaled >> 8) & 0xFF;
  buffer[3] = rpmB_scaled & 0xFF;

  Wire.write(buffer, 4);
}

void receiveData(int byteCount) {
  if (byteCount < 5) return;

  uint8_t discard = Wire.read();  // register byte (unused)
  motorMode = Wire.read();        // mode: 0x44 or 0x55

  vel_l.raw[1] = Wire.read();
  vel_l.raw[0] = Wire.read();
  vel_r.raw[1] = Wire.read();
  vel_r.raw[0] = Wire.read();

  motorSpeed.leftSpeed = vel_l.value;
  motorSpeed.rightSpeed = vel_r.value;
  lastI2CReceiveTime = millis();
}

// === Setup & Loop ===
void setup() {
  initMotors();
  initEncoders();

  Wire.begin(I2C_SLAVE_ADDRESS, I2C_SDA, I2C_SCL, 0);
  Wire.onReceive(receiveData);
  Wire.onRequest(onI2CRequest);

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
      motorMode = 0x00;
    }
  }

  if (now - lastRPMTime >= interval) {
    lastRPMTime = now;

    rpm_B = ((double)B_wheel_pulse_count / shaft_ppr) * 60000.0 / interval;
    rpm_A = ((double)A_wheel_pulse_count / shaft_ppr) * 60000.0 / interval;
    B_wheel_pulse_count = 0;
    A_wheel_pulse_count = 0;
  }

  delay(10);
}
