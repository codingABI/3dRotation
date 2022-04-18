/*
 * Project: 3dRotation
 * Description: A rotating 3d object on an arduino UNO with a SSD1306 OLED 128x64 pixel display.
 * License: CC0
 * 
 * created by codingABI https://github.com/codingABI/3dRotation (inspired by https://wokwi.com/projects/328271658006610514)
 * 
 * History:
 * 15.04.2022, Initial version
 * 16.04.2022, Improve isBackfaceRect
 * 17.04.2022, Fix degree 360 and change rotation based on millis
 * 18.04.2022, Reduce global RAM consumtion by using PROGMEM for the 3d object
 * 18.04.2022, Add three alternative 3d objects
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "object_default.h"
//#include "object_spaceship.h" // alternative 3d model
//#include "object_cubes.h" // another alternative 3d model
//#include "object_ball.h" // just another alternative 3d model

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306 I2C 
#define OLED_RESET     -1 // no reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define X 0
#define Y 1
#define Z 2

#define DRAWONLYLINES // do not fill polygons with black. is faster, but can causes artifacts on complex objects. Comment out this line, if you want to fill polygons 

// global variables
int viewerDistance = -200; // z-distance between 0/0/0-point and viewer/camera 
int viewerScale = 60; // 2d scale

// transformed points
signed char pointsTransformed3d[MAXPOINTS][3];

// projection for x to 2d
byte x3dTo2D (signed char x, signed char z) {
    if (z+viewerDistance != 0) {
      return (float) 64 + viewerScale * x/(z+viewerDistance);
    } else return 0;
}

// projection for y to 2d
byte y3dTo2D (signed char y, signed char  z) {
    if (z+viewerDistance != 0) {
      return (float) 32 + viewerScale * y/(z+viewerDistance);
    } else return 0;
}

// detect backsides for triangles (clockwise = backside)
bool isBackfaceTriangle(int i) {
  int Ax, Ay, Bx, By;

  Ax = x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][X], 
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]) - 
    x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][X],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]);
  Ay = y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Y], 
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]) - 
    y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Y],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]);

  Bx = x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][X],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]) - 
    x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][X],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]);
  By = y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Y],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]) - 
    y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Y],
    pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]);

  return ((Ax*By - Ay*Bx) < 0);  
}

// detect backsides for rectangles (clockwise = backside, based on idea from https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order)
bool isBackfaceRect(int i) {
  long sum=0;
  for (byte j=0;j<4;j++) {
    sum+=(x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][X],
      pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Z])-
      x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][X],
      pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Z]))*
      (y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Y],
      pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Z])+
      y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Y],
      pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Z]));
  }
  return (sum >= 0);
}

void setup(void) {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed")); // Adafruit_SSD1306 needs ~ 59% free dynamic memory, otherwise fails 
    for(;;); // Don't proceed, loop forever
  }

  // Font settings
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void loop(void) {
  char strFPS[8];
  float cachedCos, cachedSin;
  unsigned long startMS, endMS;
  static unsigned int fps = 0;
  static int degree = 0;

  // rotate every 25ms one degree
  degree = (millis()/25) % 360;

  // record start of frame
  startMS = millis();

  // clear display buffer
  display.clearDisplay();

  // rotate by y and z axis and move by x
  cachedCos = cos(radians(degree));
  cachedSin = sin(radians(degree));
  for (byte i=0;i<MAXPOINTS;i++) {
    // "(signed char)pgm_read_byte(&(.." stuff only needed because of PROGMEM-use for points3d
    pointsTransformed3d[i][X] = ((signed char)pgm_read_byte(&(points3d[i][X]))*cachedCos - (signed char)pgm_read_byte(&(points3d[i][Z]))*cachedSin)*cachedCos - (signed char)pgm_read_byte(&(points3d[i][Y]))*cachedSin + cachedSin * 30;
    pointsTransformed3d[i][Y] = ((signed char)pgm_read_byte(&(points3d[i][X]))*cachedCos - (signed char)pgm_read_byte(&(points3d[i][Z]))*cachedSin)*cachedSin + (signed char)pgm_read_byte(&(points3d[i][Y]))*cachedCos;
    pointsTransformed3d[i][Z] = (signed char)pgm_read_byte(&(points3d[i][X]))*cachedSin + (signed char)pgm_read_byte(&(points3d[i][Z]))*cachedCos; 
  }

  // draw triangles
  for (byte i=0;i<MAXTRIANGLES;i++) {
    if (!isBackfaceTriangle(i)) { // only front sides  

      #ifndef DRAWONLYLINES
      // fill with black/delete area
      display.fillTriangle(
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]),
        SSD1306_BLACK);
      #endif

      // draw outer border
      display.drawTriangle(
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][0]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][1]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[i][2]))][Z]),
        SSD1306_WHITE);
    }
  }

  // draw rectangles
  for (byte i=0;i<MAXRECTS;i++) {
    if (!isBackfaceRect(i)) { // only front sides

      #ifndef DRAWONLYLINES
      // fill with black/delete area
      display.fillTriangle(
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][1]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][1]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][1]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][1]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Z]),
        SSD1306_BLACK);
      display.fillTriangle(
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][0]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][3]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][3]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][3]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][3]))][Z]),
        x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Z]),
        y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][2]))][Z]),
        SSD1306_BLACK);
      #endif

      // draw outer border
      for (byte j=0;j<4;j++) {
        display.drawLine(
          x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Z]),
          y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][j]))][Z]),
          x3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][X],pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Z]),
          y3dTo2D(pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Y],pointsTransformed3d[pgm_read_byte(&(rectList[i][(j+1)%4]))][Z]), 
          SSD1306_WHITE);
      }
    }
  }

  // show frames per second
  if (fps < 1000) sprintf(strFPS,"fps %d",fps); else strcpy(strFPS,"fps inv");
  display.setCursor(0,55);
  display.println(strFPS);

  // show display buffer on screen
  display.display();

  // calculate frames per second
  endMS = millis();
  if (endMS - startMS > 0) fps = 1000/(endMS - startMS);
}
