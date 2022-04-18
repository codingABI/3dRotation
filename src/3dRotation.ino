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
 * 18.04.2022, Reduce global RAM consumtion by 40 bytes
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306 I2C 
#define OLED_RESET     -1 // no reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define P1 0
#define P2 1
#define P3 2
#define P4 3
#define P5 4
#define P6 5
#define P7 6
#define P8 7
#define P9 8
#define P10 9
#define X 0
#define Y 1
#define Z 2
#define MAXPOINTS 10
#define MAXTRIANGLES 8
#define MAXRECTS 4

// global variables
int viewerDistance = -200; // z-distance between 0/0/0-point and viewer/camera 
int viewerScale = 60; // 2d scale

// original points x,y,z
signed char points3d[MAXPOINTS][3] = {
  {-32,32,32},
  {32,32,32},
  {32,-32,32},
  {-32,-32,32},
  {-32,32,-32},
  {32,32,-32},
  {32,-32,-32},
  {-32,-32,-32},
  {0,96,0},
  {0,-96,0}
};

// transformed points
signed char pointsTransformed3d[MAXPOINTS][3];
// List of pointers to points
byte triangleList[MAXTRIANGLES][3];
byte rectList[MAXRECTS][4];

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

  Ax = x3dTo2D(pointsTransformed3d[triangleList[i][0]][X],
    pointsTransformed3d[triangleList[i][0]][Z]) - x3dTo2D(pointsTransformed3d[triangleList[i][1]][X],
    pointsTransformed3d[triangleList[i][1]][Z]);
  Ay = y3dTo2D(pointsTransformed3d[triangleList[i][0]][Y],
    pointsTransformed3d[triangleList[i][0]][Z]) - y3dTo2D(pointsTransformed3d[triangleList[i][1]][Y],
    pointsTransformed3d[triangleList[i][1]][Z]);

  Bx = x3dTo2D(pointsTransformed3d[triangleList[i][0]][X],
    pointsTransformed3d[triangleList[i][0]][Z]) - x3dTo2D(pointsTransformed3d[triangleList[i][2]][X],
    pointsTransformed3d[triangleList[i][2]][Z]);
  By = y3dTo2D(pointsTransformed3d[triangleList[i][0]][Y],
    pointsTransformed3d[triangleList[i][0]][Z]) - y3dTo2D(pointsTransformed3d[triangleList[i][2]][Y],
    pointsTransformed3d[triangleList[i][2]][Z]);

  return ((Ax*By - Ay*Bx) < 0);  
}

// detect backsides for rectangles (clockwise = backside, based on idea from https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order)
bool isBackfaceRect(int i) {
  long sum=0;
  for (byte j=0;j<4;j++) {
    sum+=(x3dTo2D(pointsTransformed3d[rectList[i][(j+1)%4]][X],pointsTransformed3d[rectList[i][(j+1)%4]][Z])-x3dTo2D(pointsTransformed3d[rectList[i][j]][X],pointsTransformed3d[rectList[i][j]][Z]))*
      (y3dTo2D(pointsTransformed3d[rectList[i][(j+1)%4]][Y],pointsTransformed3d[rectList[i][(j+1)%4]][Z])+y3dTo2D(pointsTransformed3d[rectList[i][j]][Y],pointsTransformed3d[rectList[i][j]][Z]));
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

  // build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

  rectList[0][0] = P4; rectList[0][1] = P3; rectList[0][2] = P2; rectList[0][3] = P1;
  rectList[1][0] = P5; rectList[1][1] = P6; rectList[1][2] = P7; rectList[1][3] = P8;
  rectList[2][0] = P2; rectList[2][1] = P3; rectList[2][2] = P7; rectList[2][3] = P6;
  rectList[3][0] = P4; rectList[3][1] = P1; rectList[3][2] = P5; rectList[3][3] = P8;

  triangleList[0][0] = P1; triangleList[0][1] = P2; triangleList[0][2] = P9;
  triangleList[1][0] = P6; triangleList[1][1] = P5; triangleList[1][2] = P9;
  triangleList[2][0] = P2; triangleList[2][1] = P6; triangleList[2][2] = P9;
  triangleList[3][0] = P5; triangleList[3][1] = P1; triangleList[3][2] = P9;
  triangleList[4][0] = P3; triangleList[4][1] = P4; triangleList[4][2] = P10;
  triangleList[5][0] = P4; triangleList[5][1] = P8; triangleList[5][2] = P10;
  triangleList[6][0] = P8; triangleList[6][1] = P7; triangleList[6][2] = P10;
  triangleList[7][0] = P7; triangleList[7][1] = P3; triangleList[7][2] = P10;
}

void loop(void) {
  char strFPS[8];
  float cachedCos, cachedSin;
  unsigned long startMS, endMS;
  static unsigned int fps = 0;
  static int degree = 0;


  // rotate every 25ms one degree
  degree = (millis()/25) % 359;

  // record start of frame
  startMS = millis();

  // clear display buffer
  display.clearDisplay();

  // rotate by y and z axis and move by x
  cachedCos = cos(radians(degree));
  cachedSin = sin(radians(degree));
  for (byte i=0;i<MAXPOINTS;i++) {
    pointsTransformed3d[i][X] = (points3d[i][X]*cachedCos - points3d[i][Z]*cachedSin)*cachedCos - points3d[i][Y]*cachedSin + cachedSin * 30;
    pointsTransformed3d[i][Y] = (points3d[i][X]*cachedCos - points3d[i][Z]*cachedSin)*cachedSin + points3d[i][Y]*cachedCos;
    pointsTransformed3d[i][Z] = points3d[i][X]*cachedSin + points3d[i][Z]*cachedCos; 
  }

  // draw triangles
  for (byte i=0;i<MAXTRIANGLES;i++) {
    if (!isBackfaceTriangle(i)) { // only front sides  
      display.drawTriangle(x3dTo2D(pointsTransformed3d[triangleList[i][0]][X],pointsTransformed3d[triangleList[i][0]][Z]),y3dTo2D(pointsTransformed3d[triangleList[i][0]][Y],pointsTransformed3d[triangleList[i][0]][Z]),
        x3dTo2D(pointsTransformed3d[triangleList[i][1]][X],pointsTransformed3d[triangleList[i][1]][Z]),y3dTo2D(pointsTransformed3d[triangleList[i][1]][Y],pointsTransformed3d[triangleList[i][1]][Z]),
        x3dTo2D(pointsTransformed3d[triangleList[i][2]][X],pointsTransformed3d[triangleList[i][2]][Z]),y3dTo2D(pointsTransformed3d[triangleList[i][2]][Y],pointsTransformed3d[triangleList[i][2]][Z]),SSD1306_WHITE);
    }
  }

  // draw rectangles
  for (byte i=0;i<MAXRECTS;i++) {
    if (!isBackfaceRect(i)) { // only front sides     
      for (byte j=0;j<4;j++) {
        display.drawLine(x3dTo2D(pointsTransformed3d[rectList[i][j]][X],pointsTransformed3d[rectList[i][j]][Z]),y3dTo2D(pointsTransformed3d[rectList[i][j]][Y],pointsTransformed3d[rectList[i][j]][Z]),
          x3dTo2D(pointsTransformed3d[rectList[i][(j+1)%4]][X],pointsTransformed3d[rectList[i][(j+1)%4]][Z]),y3dTo2D(pointsTransformed3d[rectList[i][(j+1)%4]][Y],pointsTransformed3d[rectList[i][(j+1)%4]][Z]), SSD1306_WHITE);
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
