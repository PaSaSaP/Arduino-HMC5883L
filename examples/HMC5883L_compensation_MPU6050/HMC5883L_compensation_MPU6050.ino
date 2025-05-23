/*
  Tilt compensated HMC5883L + MPU6050 (GY-86 / GY-87). Output for HMC5883L_compensation_processing.pde
  Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-magnetometr-hmc5883l.html
  GIT: https://github.com/jarzebski/Arduino-HMC5883L
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <HMC5883L.h>
#include <MPU6050.h>

HMC5883L compass;
MPU6050 mpu;

float heading1;
float heading2;

int currOffX = -1604;
int currOffY = -422;

void setup()
{
  Serial.begin(57600);

  // Initialize MPU6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    delay(500);
  }

  // Enable bypass mode
  //    mpu.setI2CMasterModeEnabled(false);
  //    mpu.setI2CBypassEnabled(true);
  //    mpu.setSleepEnabled(false);

  // Initialize Initialize HMC5883L
  while (!compass.begin())
  {
    delay(500);
  }

  // Set measurement range
//  compass.setRange(HMC5883L_RANGE_2GA);

  // Set measurement mode
//  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
//  compass.setDataRate(HMC5883L_DATARATE_50HZ);

  // Set number of samples averaged
//  compass.setSamples(HMC5883L_SAMPLES_512);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(currOffX, currOffY);

  mpu.setAccelOffsetX(1609);
  mpu.setAccelOffsetY(1261);
  mpu.setAccelOffsetZ(2021);
  
  mpu.setGyroOffsetX(-94);
  mpu.setGyroOffsetY(-259);
  mpu.setGyroOffsetZ(63);
}

// No tilt compensation
float noTiltCompensate(Vector mag)
{
  float heading = atan2(mag.YAxis, mag.XAxis);
  return heading;
}

// Tilt compensation
float tiltCompensate(Vector mag, Vector normAccel)
{
  // Pitch & Roll

  float roll;
  float pitch;

  roll = asin(normAccel.YAxis);
  pitch = asin(-normAccel.XAxis);

  if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78)
  {
    return -1000;
  }

  // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(roll);
  float sinRoll = sin(roll);
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);

  // Tilt compensation
  float Xh = mag.XAxis * cosPitch + mag.ZAxis * sinPitch;
  float Yh = mag.XAxis * sinRoll * sinPitch + mag.YAxis * cosRoll - mag.ZAxis * sinRoll * cosPitch;

  float heading = atan2(Yh, Xh);

  return heading;
}

// Correct angle
float correctAngle(float heading)
{
  if (heading < 0) {
    heading += 2 * PI;
  }
  if (heading > 2 * PI) {
    heading -= 2 * PI;
  }

  return heading;
}

void loop()
{
  // Read vectors
  Vector mag = compass.readNormalize();
  Vector acc = mpu.readScaledAccel();
  //  Vector acc = mpu.readNormalizeAccel();

  // Calculate headings
  heading1 = noTiltCompensate(mag);
  heading2 = tiltCompensate(mag, acc);

  char h = '+';
  if (heading2 == -1000)
  {
    heading2 = heading1;
    h = '-';
  }

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (6.0 + (33.0 / 60.0)) / (180 / M_PI);
  heading1 += declinationAngle;
  heading2 += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  heading1 = correctAngle(heading1);
  heading2 = correctAngle(heading2);

  // Convert to degrees
  heading1 = heading1 * 180 / M_PI;
  heading2 = heading2 * 180 / M_PI;

  // Output
  Serial.print(heading1);
  Serial.print(":");
  Serial.print(heading2);
//  Serial.print(":");
//  Serial.print(h);

  //  auto s = mpu.readRawAccel();
//  Serial.print(":");
//  Serial.print(acc.XAxis); Serial.print(":");
//  Serial.print(acc.YAxis); Serial.print(":");
//  Serial.print(acc.ZAxis); Serial.print(":");

  Serial.println();

  delay(100);
}
