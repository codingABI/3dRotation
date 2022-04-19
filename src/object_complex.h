// overlapping more complex object

#undef DRAWONLYLINES

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
#define P19 18
#define P20 19
#define P21 20
#define P22 21
#define P23 22
#define P24 23
#define MAXPOINTS 24
const PROGMEM signed char points3d[MAXPOINTS][3] = {
  {-16,16,16},
  {16,16,16},
  {16,-16,16},
  {-16,-16,16},
  {-16,16,-16},
  {16,16,-16},
  {16,-16,-16},
  {-16,-16,-16},

  {-48,48,-48},
  {48,48,-48},
  {48,-48,-48},
  {-48,-48,-48},
  {-48,48,-50},
  {48,48,-50},
  {48,-48,-50},
  {-48,-48,-50},

  {-48,48,48},
  {48,48,48},
  {48,-48,48},
  {-48,-48,48},
  {-48,48,50},
  {48,48,50},
  {48,-48,50},
  {-48,-48,50}

};

// build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

// List of arranged points for triangles and rectangles
#define MAXTRIANGLES 0
const PROGMEM byte triangleList[MAXTRIANGLES][3] {};

#define MAXRECTS 18
const PROGMEM byte rectList[MAXRECTS][4] {
  { P4, P3, P2, P1 },
  { P5, P6,  P7,  P8 },
  { P1, P2,  P6,  P5 },
  { P8, P7,  P3,  P4 },
  { P3, P7,  P6,  P2 },
  { P4, P1, P5, P8 },
  { P12, P11, P10, P9 },
  { P13, P14,  P15,  P16 },
  { P9, P10,  P14,  P13 },
  { P16, P15,  P11,  P12 },
  { P11, P15,  P14,  P10 },
  { P12, P9, P13, P16 },
  { P20, P19, P18, P17 },
  { P21, P22,  P23,  P24 },
  { P17, P18,  P22,  P21 },
  { P24, P23,  P19,  P20 },
  { P19, P23,  P22,  P18 },
  { P20, P17, P21, P24 }
};
