// ball

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
#define P11 10
#define P12 11
#define MAXPOINTS 12
const PROGMEM signed char points3d[MAXPOINTS][3] = {
  {-64,0,32},
  {64,0,32},
  {64,0,-32}, 
  {-64,0,-32},
  {0,32,64},
  {0,32,-64},
  {0,-32,-64},
  {0,-32,64},
  {-32,64,0},
  {32,64,0},
  {32,-64,0},
  {-32,-64,0}
};

// build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

// List of arranged points for triangles and rectangles
#define MAXTRIANGLES 20
const PROGMEM byte triangleList[MAXTRIANGLES][3] {
  { P1,P9,P4},
  { P9,P6,P4},
  { P1,P5,P9},
  { P5,P2,P10},
  { P8,P5,P1},
  { P8,P1,P12},
  { P5, P10, P9 },
  { P12,P1,P4},
  { P7, P4, P6 },
  { P7, P6, P3 },
  { P7, P12, P4 },
  { P7, P11, P12 },
  { P7, P3, P11 }, 
  { P3, P10, P2 },
  { P11, P2, P8 },
  { P11, P8, P12 },
  { P3, P2, P11 },
  { P3, P6, P10 },
  { P2, P5, P8 },
  { P6, P9, P10 }
};

#define MAXRECTS 0
const PROGMEM byte rectList[MAXRECTS][4] {};
