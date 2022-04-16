/*
 * Project: 3dRotation
 * Description: A rotating 3d object on an ardunino uno with a SSD1306 OLED display.
 * License: CC0
 * 
 * created by codingABI (inspired by https://wokwi.com/projects/328271658006610514)
 * 
 * History:
 * 15.04.2022, Initial version
 * 16.04.2022, Improve isBackfaceRect
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306 I2C 
#define OLED_RESET     -1 // no eset pin
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
int viewerDistance = -200; // z-distance between center and viewer/camera 
int viewerScale = 60; // 2d scale

// points x,y,z
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

signed char pointsRotated3d[MAXPOINTS][3];
// List of pointers to points
signed char* triangles3d[MAXTRIANGLES][3];
signed char* rects3d[MAXRECTS][4];

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

  Ax = x3dTo2D(triangles3d[i][0][X],triangles3d[i][0][Z]) - x3dTo2D(triangles3d[i][1][X],triangles3d[i][1][Z]);
  Ay = y3dTo2D(triangles3d[i][0][Y],triangles3d[i][0][Z]) - y3dTo2D(triangles3d[i][1][Y],triangles3d[i][1][Z]);

  Bx = x3dTo2D(triangles3d[i][0][X],triangles3d[i][0][Z]) - x3dTo2D(triangles3d[i][2][X],triangles3d[i][2][Z]);
  By = y3dTo2D(triangles3d[i][0][Y],triangles3d[i][0][Z]) - y3dTo2D(triangles3d[i][2][Y],triangles3d[i][2][Z]);

  return ((Ax*By - Ay*Bx) < 0);  
}

// detect backsides for rectangles (clockwise = backside, based on idea from https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order)
bool isBackfaceRect(int i) {
  long sum=0;
  for (byte j=0;j<3;j++) {
    sum+=(x3dTo2D(rects3d[i][j+1][X],rects3d[i][j+1][Z])-x3dTo2D(rects3d[i][j][X],rects3d[i][j][Z]))
      *(y3dTo2D(rects3d[i][j+1][Y],rects3d[i][j+1][Z])+y3dTo2D(rects3d[i][j][Y],rects3d[i][j][Z]));
  }
  sum+=(x3dTo2D(rects3d[i][0][X],rects3d[i][0][Z])-x3dTo2D(rects3d[i][3][X],rects3d[i][3][Z]))     
    *(y3dTo2D(rects3d[i][0][Y],rects3d[i][0][Z])+y3dTo2D(rects3d[i][3][Y],rects3d[i][3][Z]));

  return (sum >= 0);
}

void setup(void) {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

  rects3d[0][0] = pointsRotated3d[P4]; rects3d[0][1] = pointsRotated3d[P3]; rects3d[0][2] = pointsRotated3d[P2]; rects3d[0][3] = pointsRotated3d[P1];
  rects3d[1][0] = pointsRotated3d[P5]; rects3d[1][1] = pointsRotated3d[P6]; rects3d[1][2] = pointsRotated3d[P7]; rects3d[1][3] = pointsRotated3d[P8];
  rects3d[2][0] = pointsRotated3d[P2]; rects3d[2][1] = pointsRotated3d[P3]; rects3d[2][2] = pointsRotated3d[P7]; rects3d[2][3] = pointsRotated3d[P6];
  rects3d[3][0] = pointsRotated3d[P4]; rects3d[3][1] = pointsRotated3d[P1]; rects3d[3][2] = pointsRotated3d[P5]; rects3d[3][3] = pointsRotated3d[P8];

  triangles3d[0][0] = pointsRotated3d[P1]; triangles3d[0][1] = pointsRotated3d[P2]; triangles3d[0][2] = pointsRotated3d[P9];
  triangles3d[1][0] = pointsRotated3d[P6]; triangles3d[1][1] = pointsRotated3d[P5]; triangles3d[1][2] = pointsRotated3d[P9];
  triangles3d[2][0] = pointsRotated3d[P2]; triangles3d[2][1] = pointsRotated3d[P6]; triangles3d[2][2] = pointsRotated3d[P9];
  triangles3d[3][0] = pointsRotated3d[P5]; triangles3d[3][1] = pointsRotated3d[P1]; triangles3d[3][2] = pointsRotated3d[P9];
  triangles3d[4][0] = pointsRotated3d[P3]; triangles3d[4][1] = pointsRotated3d[P4]; triangles3d[4][2] = pointsRotated3d[P10];
  triangles3d[5][0] = pointsRotated3d[P4]; triangles3d[5][1] = pointsRotated3d[P8]; triangles3d[5][2] = pointsRotated3d[P10];
  triangles3d[6][0] = pointsRotated3d[P8]; triangles3d[6][1] = pointsRotated3d[P7]; triangles3d[6][2] = pointsRotated3d[P10];
  triangles3d[7][0] = pointsRotated3d[P7]; triangles3d[7][1] = pointsRotated3d[P3]; triangles3d[7][2] = pointsRotated3d[P10];
}

void loop(void) {
  char strFPS[8];
  float cachedCos, cachedSin;
  unsigned long startMS, endMS;
  static int fps = 0;
  static int degree = 0;

  startMS = millis();

  // rotation degree per display frame
  degree = degree + 2;
  if (degree > 360) degree = 0;

  // clear display buffer
  display.clearDisplay();

  // rotate by y and z axis and move by x
  cachedCos = cos(radians(degree));
  cachedSin = sin(radians(degree));
  for (byte i=0;i<MAXPOINTS;i++) {
    pointsRotated3d[i][X] = (points3d[i][X]*cachedCos - points3d[i][Z]*cachedSin)*cachedCos - points3d[i][Y]*cachedSin + cachedSin * 30;
    pointsRotated3d[i][Y] = (points3d[i][X]*cachedCos - points3d[i][Z]*cachedSin)*cachedSin + points3d[i][Y]*cachedCos;
    pointsRotated3d[i][Z] = points3d[i][X]*cachedSin + points3d[i][Z]*cachedCos; 
  }

  // draw triangles
  for (byte i=0;i<MAXTRIANGLES;i++) {
    if (!isBackfaceTriangle(i)) { // only front sides  
      display.drawTriangle(x3dTo2D(triangles3d[i][0][X],triangles3d[i][0][Z]),y3dTo2D(triangles3d[i][0][Y],triangles3d[i][0][Z]),
        x3dTo2D(triangles3d[i][1][X],triangles3d[i][1][Z]),y3dTo2D(triangles3d[i][1][Y],triangles3d[i][1][Z]),
        x3dTo2D(triangles3d[i][2][X],triangles3d[i][2][Z]),y3dTo2D(triangles3d[i][2][Y],triangles3d[i][2][Z]),SSD1306_WHITE);
    }
  }

  // draw rectangles
  for (byte i=0;i<MAXRECTS;i++) {
    if (!isBackfaceRect(i)) { // only front sides 
      display.drawLine(x3dTo2D(rects3d[i][0][X],rects3d[i][0][Z]),y3dTo2D(rects3d[i][0][Y],rects3d[i][0][Z]),
        x3dTo2D(rects3d[i][1][X],rects3d[i][1][Z]),y3dTo2D(rects3d[i][1][Y],rects3d[i][1][Z]), SSD1306_WHITE);
      display.drawLine(x3dTo2D(rects3d[i][1][X],rects3d[i][1][Z]),y3dTo2D(rects3d[i][1][Y],rects3d[i][1][Z]),
        x3dTo2D(rects3d[i][2][X],rects3d[i][2][Z]),y3dTo2D(rects3d[i][2][Y],rects3d[i][2][Z]), SSD1306_WHITE);
      display.drawLine(x3dTo2D(rects3d[i][2][X],rects3d[i][2][Z]),y3dTo2D(rects3d[i][2][Y],rects3d[i][2][Z]),
        x3dTo2D(rects3d[i][3][X],rects3d[i][3][Z]),y3dTo2D(rects3d[i][3][Y],rects3d[i][3][Z]),SSD1306_WHITE);
      display.drawLine(x3dTo2D(rects3d[i][3][X],rects3d[i][3][Z]),y3dTo2D(rects3d[i][3][Y],rects3d[i][3][Z]),
        x3dTo2D(rects3d[i][0][X],rects3d[i][0][Z]),y3dTo2D(rects3d[i][0][Y],rects3d[i][0][Z]), SSD1306_WHITE);
    }
  }

  // show frames per second
  sprintf(strFPS,"fps %d",fps);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,55);
  display.println(strFPS);

  // show display buffer on screen
  display.display();

  endMS = millis();
  fps = 1000/(endMS - startMS);
}
