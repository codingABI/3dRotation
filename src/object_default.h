// Default 3d object

// original points x,y,z
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
#define MAXPOINTS 10
const PROGMEM signed char points3d[MAXPOINTS][3] = {
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

// build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

// List of arranged points for triangles and rectangles
#define MAXTRIANGLES 8
const PROGMEM byte triangleList[MAXTRIANGLES][3] {
  { P1, P2, P9 },
  { P6, P5, P9 },
  { P2, P6, P9 },
  { P5, P1, P9 },
  { P3, P4, P10 },
  { P4, P8, P10 },
  { P8, P7, P10 },
  { P7, P3, P10 }
};

#define MAXRECTS 4
const PROGMEM byte rectList[MAXRECTS][4] {
  { P4, P3, P2, P1 },
  { P5, P6, P7, P8 },
  { P2, P3, P7, P6 },
  { P4, P1, P5, P8 }
};
