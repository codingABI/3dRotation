// More detailed object (spaceship like)
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
#define P13 12
#define P14 13
#define P15 14
#define P16 15
#define P17 16
#define P18 17
#define MAXPOINTS 18
const PROGMEM signed char points3d[MAXPOINTS][3] = {
  {-16,32,32},
  {16,32,32},
  {16,-32,32},
  {-16,-32,32},
  {-16,32,-32},
  {16,32,-32},
  {16,-32,-32},
  {-16,-32,-32},
  {0,90,-32},
  {0,-90,-32},
  {-4,4,64},
  {4,4,64},
  {4,-4,64},
  {-4,-4,64},
  {-8,8,-40},
  {8,8,-40},
  {8,-8,-40},
  {-8,-8,-40 }
};

// build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

// List of arranged points for triangles and rectangles
#define MAXTRIANGLES 8
byte triangleList[MAXTRIANGLES][3] {
  {P1, P2, P9 },
  {P6, P5, P9 },
  {P2, P6, P9 },
  {P5, P1, P9 },
  {P3, P4, P10 },
  {P4, P8, P10 },
  {P8, P7, P10 },
  {P7, P3, P10 }
};

#define MAXRECTS 12 
byte rectList[MAXRECTS][4] {
  { P14, P13, P12, P11 },
  { P14, P4,  P3,  P13 },
  { P13, P3,  P2,  P12 },
  { P12, P2,  P1,  P11 },
  { P11, P1,  P4,  P14 },
  { P15, P16, P17, P18 },
  { P18, P17, P7,  P8 },
  { P17, P16, P6,  P7 },
  { P16, P15, P5,  P6 },
  { P15, P18, P8,  P5 },
  { P2,  P3,  P7,  P6 },
  { P4,  P1,  P5,  P8 }
};
