#include"IMU.h"

//Set the values for the Roll, Pitch, and Yaw corners, as well as the Temp temperature.
//Roll represents the roll angle of rotation around the X-axis, Pitch represents the pitch angle of rotation around the Y-axis, and Yaw represents the yaw angle of rotation around the Z-axis.
int   IMU_Roll = 100;
int   IMU_Pitch = 100;
int   IMU_Yaw = 100;
int   IMU_Temp = 100;

IMU_ST_ANGLES_DATA stAngles;    //Create structure variable stAngles to store the three angle data
IMU_ST_SENSOR_DATA stGyroRawData;    //For storing raw gyroscope data
IMU_ST_SENSOR_DATA stAccelRawData;    //Stores raw accelerometer data
IMU_ST_SENSOR_DATA stMagnRawData;    //Storing magnetometer raw data

void getIMU(){
  //Obtain IMU data
  imuDataGet( &stAngles, &stGyroRawData, &stAccelRawData, &stMagnRawData);
  IMU_Temp = QMI8658_readTemp();
  IMU_Roll  = stAngles.fRoll;
  IMU_Pitch = stAngles.fPitch;
  IMU_Yaw   = stAngles.fYaw;
}

void DOF(){
  Serial.print("R:");
  Serial.println(IMU_Roll);
  Serial.print("P:");
  Serial.println(IMU_Pitch);
  Serial.print("Y:");
  Serial.println(IMU_Yaw);
  Serial.print("T:");
  Serial.println(IMU_Temp);

}

void setup() {

  Serial.begin(115200);    //Initializing Serial Communications
  while(!Serial){}    //Waiting for serial port connection
  imuInit();    //Initialize IMU
} 


void loop() {
  getIMU();    //Obtain IMU data 
  DOF();    //Output IMU data 
  delay(1000);
}