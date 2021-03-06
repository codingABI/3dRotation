/*
 * Project: 3dRotation
 * Description: A rotating 3d object on an arduino UNO/NANO with a SSD1306 OLED 128x64 pixel display.
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
 * 19.04.2022, Add ordered display list to support overlapping objects (still glitches due perspective and large polygons, but usable and I have currently no better solution without z-buffer)
 * 20.04.2022, Improve object_complex.h (more smaller triangles, less glitches when overlapping)
 * 21.04.2022, Fix bug in ordering display list
 * 27.05.2022, Release version v0.1.0
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // dont forget to uncomment #define SSD1306_NO_SPLASH in Adafruit_SSD1306.h to free program storage

#define DRAWONLYLINES // do not fill polygons with black. is faster, but can causes artifacts on complex objects. #undef in object_*.h file, if you want to fill polygons 

// Include always only one 3d object_*.h-File at a time
#include "object_default.h" // default 3d object
//#include "object_spaceship.h" // alternative 3d model
//#include "object_cubes.h" // another alternative 3d model
//#include "object_icosahedron.h" // just another alternative 3d model
//#include "object_complex.h" // overlapping 3d model

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306 I2C 
#define OLED_RESET -1 // no reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define X 0
#define Y 1
#define Z 2

#define TYPE_TRIANGLE 2
#define TYPE_QUADRANGLE 1

// global variables
int viewerDistance = 200; // z-value for viewer/camera 
int viewerScale = 60; // 2d scale

// transformed points
signed char pointsTransformed3d[MAXPOINTS][3];

// ordered display list
struct displayListEntry {
  byte type;
  byte nbr;
  signed char minZ;
  signed char maxZ;
};
displayListEntry displayList[MAXTRIANGLES+MAXQUADRANGLES];

// projection for x to 2d
int x3dTo2D (signed char x, signed char z) {
  if (z-viewerDistance != 0) {
    return (float) SCREEN_WIDTH/2 + viewerScale * x/(z-viewerDistance);
  } else return 0;
}

// projection for y to 2d
int y3dTo2D (signed char y, signed char  z) {
  if (z-viewerDistance != 0) {
    return (float) SCREEN_HEIGHT/2 - viewerScale * y/(z-viewerDistance);
  } else return 0;
}

// detect backsides for polygons (clockwise = backside, based on idea from https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order)
bool isBackface(byte type, int i) {
  long sum=0;
  switch (type) {
    case TYPE_TRIANGLE:
      for (byte j=0;j<3;j++) {
        sum+=(x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][(j+1)%3]))][X],
          pointsTransformed3d[pgm_read_byte(&(triangleList[i][(j+1)%3]))][Z])-
          x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][j]))][X],
          pointsTransformed3d[pgm_read_byte(&(triangleList[i][j]))][Z]))*
          (y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][(j+1)%3]))][Y],
          pointsTransformed3d[pgm_read_byte(&(triangleList[i][(j+1)%3]))][Z])+
          y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[i][j]))][Y],
          pointsTransformed3d[pgm_read_byte(&(triangleList[i][j]))][Z]));
      }
    break;
    case TYPE_QUADRANGLE:
      for (byte j=0;j<4;j++) {
        sum+=(x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][(j+1)%4]))][X],
          pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][(j+1)%4]))][Z])-
          x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][j]))][X],
          pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][j]))][Z]))*
          (y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][(j+1)%4]))][Y],
          pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][(j+1)%4]))][Z])+
          y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][j]))][Y],
          pointsTransformed3d[pgm_read_byte(&(quadrangleList[i][j]))][Z]));
      }
    break;
  }
  // sum < 0 is counterclockwise in xy-coordinatesystem, but clockwise in screen-coordinatesystem 
  // sum = 0 is when the polygon collaps just to a single line
  return (sum <= 0);
}

// get highest z-value from polygon
signed char getMaxZ(byte type, byte i) {
  signed char maxZ = -128;

  switch (type) {
    case TYPE_TRIANGLE:
      for (byte j=0;j<3;j++) if (pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][j]))][Z] > maxZ) maxZ = pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][j]))][Z];
    break;
    case TYPE_QUADRANGLE:
      for (byte j=0;j<4;j++) if (pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z] > maxZ) maxZ = pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z];
    break;
  }  
  return  maxZ;
}

// get lowest z-value from polygon
signed char getMinZ(byte type, byte i) {
  signed char minZ = 127;

  switch (type) {
    case TYPE_TRIANGLE:
      for (byte j=0;j<3;j++) if (pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][j]))][Z] < minZ) minZ = pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][j]))][Z];
    break;
    case TYPE_QUADRANGLE:
      for (byte j=0;j<4;j++) if (pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z] < minZ) minZ = pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z];
    break;
  }  
  return  minZ;
}

// sort display list by z-value
void sortDisplayList() {
  displayListEntry tempEntry;
  // bouble sort
  for (byte i=0;i<MAXTRIANGLES+MAXQUADRANGLES - 1;i++) {
    for (byte j=0;j<MAXTRIANGLES+MAXQUADRANGLES-i-1;j++) {
      if ((displayList[j].minZ > displayList[j+1].minZ) || ((displayList[j].minZ == displayList[j+1].minZ) && (displayList[j].maxZ > displayList[j+1].maxZ))) {
        tempEntry = displayList[j];
        displayList[j] = displayList[j+1];
        displayList[j+1] = tempEntry;
      }
    }
  }
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
  int degree;

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

  // add triangles to display list
  for (byte i=0;i<MAXTRIANGLES;i++) {
    displayList[i].type = TYPE_TRIANGLE;
    displayList[i].nbr = i;
    displayList[i].minZ = getMinZ(TYPE_TRIANGLE,i);
    displayList[i].maxZ = getMaxZ(TYPE_TRIANGLE,i);
  }
  // add quadrangles to display list
  for (byte i=0;i<MAXQUADRANGLES;i++) {
    displayList[MAXTRIANGLES+i].type = TYPE_QUADRANGLE;
    displayList[MAXTRIANGLES+i].nbr = i;
    displayList[MAXTRIANGLES+i].minZ = getMinZ(TYPE_QUADRANGLE,i);
    displayList[MAXTRIANGLES+i].maxZ = getMaxZ(TYPE_QUADRANGLE,i);
  }

  // sort display list by z-value
  sortDisplayList();

  // draw display list
  for (byte i=0;i<MAXTRIANGLES+MAXQUADRANGLES;i++) {
    if (!isBackface(displayList[i].type,displayList[i].nbr)) { // only front sides  
      switch(displayList[i].type) {
        case TYPE_TRIANGLE: // draw triangle
          #ifndef DRAWONLYLINES
          // fill with black/delete area
          display.fillTriangle(
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Z]),
            SSD1306_BLACK);
          #endif
          // draw outer border
          display.drawTriangle(
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][0]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][1]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][X],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Y],pointsTransformed3d[pgm_read_byte(&(triangleList[displayList[i].nbr][2]))][Z]),
            SSD1306_WHITE);
        break;
        case TYPE_QUADRANGLE : // draw rectangle
          #ifndef DRAWONLYLINES
          // fill with black/delete area
          display.fillTriangle(
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][1]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][1]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][1]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][1]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Z]),
            SSD1306_BLACK);
          display.fillTriangle(
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][0]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][3]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][3]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][3]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][3]))][Z]),
            x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Z]),
            y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][2]))][Z]),
            SSD1306_BLACK);
          #endif
          // draw outer border
          for (byte j=0;j<4;j++) {
            display.drawLine(
              x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z]),
              y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][j]))][Z]),
              x3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][(j+1)%4]))][X],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][(j+1)%4]))][Z]),
              y3dTo2D(pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][(j+1)%4]))][Y],pointsTransformed3d[pgm_read_byte(&(quadrangleList[displayList[i].nbr][(j+1)%4]))][Z]), 
              SSD1306_WHITE);
          }
        break;
      }
    }
  }

  // show frames per second
  if (fps < 1000) sprintf(strFPS,"fps %2d",fps); else strcpy(strFPS,"fps inv");
  display.setCursor(0,55);
  display.println(strFPS);

  // show display buffer on screen
  display.display();

  // calculate frames per second
  endMS = millis();
  if (endMS - startMS > 0) fps = 1000/(endMS - startMS);
}
