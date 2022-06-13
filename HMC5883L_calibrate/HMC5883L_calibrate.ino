/*
  Calibrate HMC5883L. Output for HMC5883L_calibrate_processing.pde
  Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-magnetometr-hmc5883l.html
  GIT: https://github.com/jarzebski/Arduino-HMC5883L
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

/*
21:33:42.191 -> .................... XAccel      YAccel        ZAccel      XGyro     YGyro     ZGyro
21:34:32.187 ->  [1609,1610] --> [-15,1]  [1261,1262] --> [-1,15] [2021,2023] --> [16375,16391] [-94,-93] --> [-2,1]  [-259,-258] --> [0,4] [63,64] --> [0,3]
21:34:34.835 -> .................... [1609,1610] --> [-16,1]  [1261,1262] --> [-2,15] [2021,2022] --> [16375,16392] [-94,-93] --> [-2,1]  [-259,-259] --> [0,1] [63,64] --> [0,3]
21:35:27.512 -> .................... [1609,1610] --> [-15,1]  [1261,1262] --> [0,15]  [2021,2022] --> [16373,16392] [-94,-93] --> [-2,1]  [-259,-259] --> [0,1] [63,64] --> [0,3]
21:36:17.566 -> -------------- done --------------

 */
 
#include <Wire.h>
#include <HMC5883L.h>

HMC5883L compass;

int minX = 0x7FFF;
int maxX = -0x7FFF;
int minY = 0x7FFF;
int maxY = -0x7FFF;
int minZ = 0x7FFF;
int maxZ = -0x7FFF;
int offX = 0;
int offY = 0;
int offZ = 0;
int scaleX = 0;
int scaleY = 0;
int scaleZ = 0;

/*
 * first qmc+mpu board
int const currOffX = -1656;
int const currOffY = -682;
int const currOffZ = -252;

int const currScaleX = 5680;
int const currScaleY = 5864;
int const currScaleZ = 5740;

int const accelOffsetX = 1609;
int const accelOffsetY = 1261;
int const accelOffsetZ = 2021;

int const gyroOffsetX = -94;
int const gyroOffsetY = -259;
int const gyroOffsetZ = 63;
*/

int const currOffX = -176;
int const currOffY = -1685;
int const currOffZ = 120;

int const currScaleX = 10597;
int const currScaleY = 11104;
int const currScaleZ = 10940;

// Data is printed as: aX aY aZ gX gY gZ
// Your offsets: -1335 2584  770 -124  -13 -33

/* 
 * 5680
 * 5864
 * 5740
 * z: 5640
*/

void setup()
{
  Wire.begin();
  Wire.setTimeout(1000);
  Serial.begin(57600);

  Serial.println("compass begin");
  // Initialize Initialize HMC5883L
  while (!compass.begin())
  {
    delay(500);
  }
  Serial.println("done");
  // Set measurement range
//  compass.setRange(HMC5883L_RANGE_8GA);

  // Set measurement mode
//  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
//  compass.setDataRate(HMC5883L_DATARATE_50HZ);

  // Set number of samples averaged
//  compass.setSamples(HMC5883L_SAMPLES_512);

  compass.setOffset(currOffX, currOffY, currOffZ);
  compass.setScale(currScaleX, currScaleY, currScaleZ);
}

int i = 0;
Vector mag;
void loop()
{
//  mag = compass.readRaw();
  mag = compass.readNormalize();
  if (mag.XAxis == 0x7FFF || mag.YAxis == 0x7FFF) {
    delay(100);
    return;
  }

  // Determine Min / Max values
  if (mag.XAxis < minX) minX = mag.XAxis;
  if (mag.XAxis > maxX) maxX = mag.XAxis;
  if (mag.YAxis < minY) minY = mag.YAxis;
  if (mag.YAxis > maxY) maxY = mag.YAxis;
  if (mag.ZAxis < minZ) minZ = mag.ZAxis;
  if (mag.ZAxis > maxZ) maxZ = mag.ZAxis;
  
  // Calculate offsets
  offX = (maxX + minX) / 2;
  offY = (maxY + minY) / 2;
  offZ = (maxZ + minZ) / 2;

  // and scale
  scaleX = maxX - minX;
  scaleY = maxY - minY;
  scaleZ = maxZ - minZ;

  Serial.print(mag.XAxis);
  Serial.print(":");
  Serial.print(mag.YAxis);
  Serial.print(":");
  Serial.print(mag.ZAxis);
  Serial.print(":");
  Serial.print(minX);
  Serial.print(":");
  Serial.print(maxX);
  Serial.print(":");
  Serial.print(minY);
  Serial.print(":");
  Serial.print(maxY);
  Serial.print(":");
  Serial.print(minZ);
  Serial.print(":");
  Serial.print(maxZ);
  Serial.print(":");
  Serial.print(offX);
  Serial.print(":");
  Serial.print(offY);
  Serial.print(":");
  Serial.print(offZ);
  Serial.print(":");
  Serial.print(scaleX);
  Serial.print(":");
  Serial.print(scaleY);
  Serial.print(":");
  Serial.print(scaleZ);
  
  Serial.print("\n");
  delay(25);
}
