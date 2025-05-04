/*
    HMC5883L Triple Axis Digital Compass.
    Processing for HMC5883L_calibrate.ino
    Processing for HMC5883L_calibrate_MPU6050.ino
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-magnetometr-hmc5883l.html
    GIT: https://github.com/jarzebski/Arduino-HMC5883L
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

import processing.serial.*;
import processing.net.*;

enum Selector {
  Client, 
  Serial,
};

Selector selector = Selector.Client;
Serial myPort;
Client client;

// Data samples
float x = 0;
float y = 0;
float z = 0;

float minX = 32767;
float maxX = -32767;
float minY = 32767;
float maxY = -32767;
float minZ = 32767;
float maxZ = -32767;
float offX = 0;
float offY = 0;
float offZ = 0;
float scaleX = 0;
float scaleY = 0;
float scaleZ = 0;

float showX = 0;
float showY = 0;
float showZ = 0;

int ws = 900;

void setup ()
{
  size(900, 900, P2D);
  background(0);
  stroke(255);
  
  strokeWeight(2);

  line(ws, 0, ws/2, ws);
  line(0, ws/2, ws, 40);

  strokeWeight(3);
  textSize(12);

  
    if (selector == Selector.Client) {
      client = new Client(this, "192.168.55.107", 12002); // IP i port serwera
    } else if (selector == Selector.Serial) {
      //myPort = new Serial(this, "/dev/cu.usbserial-A50285BI", 57600);
      myPort = new Serial(this, "COM10", 57600);
      myPort.bufferUntil(10);
    }
}

void draw() 
{
  if (selector == Selector.Client) {
    readDataFromTcp();
  }
  stroke(255);
  strokeWeight(0);

  fill(0);  // Set fill to black
  rect(0, 0, 240, 65); 

  strokeWeight(2);
  fill(255);  // Set fill to white
  rect(ws/2-5, ws/2-5, 10, 10);//center point
  text("X: "+minX+" "+maxX+" = "+offX, 10, 20);
  text("Y: "+minY+" "+maxY+" = "+offY, 10, 35);
  text("Z: "+minZ+" "+maxZ+" = "+offZ, 10, 50);
  text("S: "+scaleX+" "+scaleY+" "+scaleZ, 10, 65);
  
  
  // if rotated make sure that showX is forward, showY is right, showZ i up 
  //showX = z;
  //showY = -x;
  //showZ = -y;
  
  showX = x;
  showY = y;
  showZ = z;
  
  point((showX*0.05)+ws/2, (showY*0.05)+ws/2);
  
  //strokeWeight(2);
  stroke(120);
  point(ws/2, (showZ*0.05)+ws/2);
}

void serialEvent (Serial myPort)
{
  String inString = myPort.readStringUntil(10);

  if (inString == null) return;
  inString = trim(inString);
  String[] list = split(inString, ':');
  
  // for use with serial
  if (list.length != 15) return;

  x = (float(list[0]));
  y = (float(list[1]));
  z = (float(list[2]));

  minX = (float(list[3]));
  maxX = (float(list[4]));
  minY = (float(list[5]));
  maxY = (float(list[6]));   
  minZ = (float(list[7]));
  maxZ = (float(list[8]));   
  offX = (float(list[9]));
  offY = (float(list[10]));  
  offZ = (float(list[11]));  
  scaleX = (float(list[12]));
  scaleY = (float(list[13]));
  scaleZ = (float(list[14]));
}

void readDataFromTcp()
{
  String inString = null;
  
  if (client.available() == 0) return;
  inString = client.readStringUntil('\n'); // lub inny separator

  if (inString == null) return;
    inString = trim(inString);
    String[] list = split(inString, ':');

    // for use with wemos board
    if (list.length != 10) return;

    x = (float(list[0]));
    y = (float(list[1]));
    z = (float(list[2]));
    // another for now unused
    
    // Aktualizacja min/max
    if (x < minX) minX = x;
    if (x > maxX) maxX = x;
    if (y < minY) minY = y;
    if (y > maxY) maxY = y;
    if (z < minZ) minZ = z;
    if (z > maxZ) maxZ = z;

    // Wyliczenie offsetów
    offX = (maxX + minX) / 2.0;
    offY = (maxY + minY) / 2.0;
    offZ = (maxZ + minZ) / 2.0;

    // Wyliczenie skal
    scaleX = maxX - minX;
    scaleY = maxY - minY;
    scaleZ = maxZ - minZ;
}
