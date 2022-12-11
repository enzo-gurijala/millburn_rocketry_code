
//microSD
#include <SD.h>
#include <SPI.h>
const int chipSelect = 4;
File myFile;

//servo
#include <Servo.h>                  
Servo myServo;

//BMP
#include <Wire.h> //SPI.h already included
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

double start_time;

#define SEALEVELPRESSURE_HPA (1025.3984882666768)//google the Barometric pressure
/*THIS WILL need to be calibrated
based on the pressure at the test site*/
Adafruit_BMP3XX bmp;


//Accelerometer
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;
//Wire.h and sensor.h already included

//buzzer
const int buzzer = 1; //buzzer to arduino pin TX

//initializing boolean to say that servo should spin once and once servo has spun, we no longer need to read BMP data and prevents servo from further spinning
bool servo_spin = true;
double alt = 0; //initializing altitude variable


void setup() {
  // put your setup code here, to run once:
//Attach servo to Digital pin 10
  
  
//BMP setup
  while (!bmp.begin_I2C());

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

//Accelerometer Setup
  
  while (!mpu.begin());
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  //Look into whether these should be changed
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

//microSD setup
  while(!SD.begin(chipSelect))


  //BUZZER (COMMENTING THIS OUT FOR NOW)
  
  pinMode(1, OUTPUT); // Set buzzer - pin TX as an output
  for(int i = 0; i<3; i++) //COMMENT THIS LINE OUT IF YOU DON'T WANT TO HEAR AN ANNOYING BUZZER SOUND. This is to let us know that the battery is properly connected by playing the buzzer.
  {  
    //tone(buzzer, 1000); // Send 1KHz sound signal...
    //delay(1000);        // ...for 1 sec
    //noTone(buzzer);     // Stop sound...
    //delay(1000);        // ...for 1sec
  }
  
  for(int i = 0; i < 4; i++) //don't have this line in the actual code (just for testing)
  {
    delay(10); //don't have this line in the actual code (just for testing)
    alt = (double) (bmp.readAltitude(SEALEVELPRESSURE_HPA)); //don't have this line in the actual code (just for testing)

  }
  delay(1000); //Delay this to when we want the rocket to start reading values (DECIDE THIS)

  start_time = millis();

}



void loop()
{

double curr_time = (millis() - start_time )/ 1000.0;
//Serial.println(curr_time);
//May want to use this if loop with a buzzer sound so that we know if the BMP isn't reading properly

//  if (! bmp.performReading()) {
//    Serial.println("Failed to perform reading :(");
//    return;
//  }

//The reason I'm writing the code this way is so that the servo will only spin once
//The point is that once the second if statement triggers and the servo spins, it will set the boolean as false and the Feather will no longer read BMP values and will no longer spin the servo.

//This delay time can be pretty low (like maybe 10-50 milliseconds), I would guess that the shorter this delay, the more data we can record and that would be good for data interpretation,
//  but more is being written on the SD card 
delay(200); //figure out what we want this to be. (this will be the time in between each iteration of the loop) (DECIDE THIS)
if(servo_spin == true)
{
  alt = bmp.readAltitude(SEALEVELPRESSURE_HPA); //may have to equal (alt-(altitude of test site))
    if(alt>=37.1)// || curr_time >= 30)//change this value for test day (probably should be lower than 8509 feet because there will still be upward momentum) (DECIDE THIS)
    {
      myServo.attach(10);
      myServo.write(10);
      servo_spin = false;
    }
}
//Accelerometer Readings
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  String dataString2 = "Rotation (X, Y, Z): ";
  
  String dataString1 = "Acceleration (X, Y, Z): ";


  dataString1 += String(a.acceleration.x);
  dataString1 += " "; //might want to change this to a "," but will keep this for now
  dataString1 += String(a.acceleration.y);
  dataString1 += " ";
  dataString1 += String(a.acceleration.z);


  dataString2 += String(g.gyro.x);
  dataString2 += " ";
  dataString2 += String(g.gyro.y);
  dataString2 += " ";
  dataString2 += String(g.gyro.z);

  
  File myFile = SD.open("rocket.log", FILE_WRITE);
  myFile.println(dataString1);
  myFile.println(dataString2);
  myFile.close();
}