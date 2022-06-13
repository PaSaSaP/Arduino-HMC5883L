/*
  Tilt compensated HMC5883L + ADXL345 (GY-80). Output for HMC5883L_compensation_processing.pde
  Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-magnetometr-hmc5883l.html
  GIT: https://github.com/jarzebski/Arduino-HMC5883L
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <HMC5883L.h>
#include <ADXL345.h>

char s[128];

HMC5883L compass;
ADXL345 accelerometer;

float heading1;
float heading2;

int currOffX = 11992;
int currOffY = 4396;

void setup()
{
  Serial.begin(57600);

  // Initialize ADXL345

  if (!accelerometer.begin())
  {
    delay(500);
  }

  accelerometer.setRange(ADXL345_RANGE_2G);

  // Initialize Initialize HMC5883L
  while (!compass.begin())
  {
    delay(500);
  }

  // Set measurement range
  compass.setRange(HMC5883L_RANGE_2GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_50HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_512);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(currOffX, currOffY);
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

  roll = asin(normAccel.YAxis); // obrót wokół iksa
  pitch = asin(-normAccel.XAxis); // obrót wokół igreka

  //  roll = asin(-normAccel.XAxis);
  //  pitch = asin(normAccel.YAxis);

  sprintf(s, "R=%2.2lf P=%2.2lf\t", roll, pitch);
  Serial.print(s);

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
  //  float Xh = mag.XAxis * cosPitch + mag.ZAxis * sinPitch;
  //  float Yh = mag.XAxis * sinRoll * sinPitch + mag.YAxis * cosRoll - mag.ZAxis * sinRoll * cosPitch;

  float Xh = mag.XAxis * cosPitch + mag.YAxis * sinRoll * sinPitch
             - mag.ZAxis * cosRoll * sinPitch;
  float Yh = mag.YAxis * cosRoll + mag.ZAxis * sinRoll;

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

int xx[32];

void loop()
{
//  for (int i = 3; i < 13; ++i) {
//    Wire.beginTransmission(HMC5883L_ADDRESS);
//    Wire.write(i);
//    Wire.endTransmission();
//    delay(10);
//    int b = 1;
//    if (i == 3 || i == 5 || i == 7) {
//      b = 2;
//    }
//    Wire.requestFrom(HMC5883L_ADDRESS, b);
//    while (!Wire.available());
//    xx[i] = Wire.read();
//    if (i == 3 || i == 5 || i == 7) {
//      i++;
//      xx[i] = Wire.read();
//    }
//    delay(10);
//  }
//  for (int i = 3; i < 13; ++i) {
//    sprintf(s, "%02X ", xx[i]);
//    Serial.print(s);
//  }
//  Serial.println();
//  delay(200);
//  return;



  // Read vectors
  Vector mag = compass.readNormalize();
  Vector accx = accelerometer.readScaled();
  Vector acc;
  acc.XAxis = accx.YAxis;
  acc.YAxis = accx.XAxis;
  acc.ZAxis = -accx.ZAxis;

  sprintf(s, "ACC: %2.1lf, %2.1lf, %2.1lf\t:", acc.XAxis, acc.YAxis, acc.ZAxis);
  Serial.print(s);

  sprintf(s, "MAG: %4.1lf, %4.1lf, %4.1lf\t:", mag.XAxis, mag.YAxis, mag.ZAxis);
  Serial.print(s);

  // Calculate headings
  heading1 = noTiltCompensate(mag);
  heading2 = tiltCompensate(mag, acc);

  if (heading2 == -1000)
  {
    heading2 = heading1;
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
  sprintf(s, "%3.2f:%3.2f\n", heading1, heading2);
  Serial.print(s);
  //  Serial.print(heading1);
  //  Serial.print(":");
  //  Serial.println(heading2);

  delay(100);
}
