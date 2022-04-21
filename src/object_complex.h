/* 
 * overlapping more complex object
 * We have only a small 3d world with x=[-128,127], y=[-128,127], z=[-128,127]
 */

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
  // Middle
  {-44,0,16},
  {0,16,16},
  {44,0,16},
  {0,-16,16},
  {-44,0,-16},
  {0,16,-16},
  {44,0,-16},
  {0,-16,-16},
  // Left
  {-48,0,64},
  {-48,48,48},
  {-48,48,-48},
  {-48,0,-64},
  {-48,-48,-48},
  {-48,-48,48},
  {-58,0,0},
  {-44,0,0},
  // Right
  {48,0,64},
  {48,48,48},
  {48,48,-48},
  {48,0,-64},
  {48,-48,-48},
  {48,-48,48},
  {44,0,0},
  {58,0,0},
};

// build mesh from points (all polygons must be defined counterclockwise, otherwise hiding of backsides will not work)

// List of ordered points for triangles and quadrangles
#define MAXTRIANGLES 24 
const PROGMEM byte triangleList[MAXTRIANGLES][3] {
  // Left
  { P9, P10, P15 },
  { P10, P11, P15 },
  { P11, P12, P15 },
  { P12, P13, P15 },
  { P13, P14, P15 },
  { P14, P9, P15 },
  { P16, P10, P9 },
  { P16, P11, P10 },
  { P16, P12, P11 },
  { P16, P13, P12 },
  { P16, P14, P13 },
  { P16, P9, P14 },
  // Right
  { P17, P18, P23 },
  { P18, P19, P23 },
  { P19, P20, P23 },
  { P20, P21, P23 },
  { P21, P22, P23 },
  { P22, P17, P23 },
  { P24, P18, P17 },
  { P24, P19, P18 },
  { P24, P20, P19 },
  { P24, P21, P20 },
  { P24, P22, P21 },
  { P24, P17, P22 },
};

#define MAXQUADRANGLES 6
const PROGMEM byte quadrangleList[MAXQUADRANGLES][4] {
  // Middle
  { P4, P3, P2, P1 }, 
  { P5, P6, P7, P8 },
  { P1, P2, P6, P5 },
  { P8, P7, P3, P4 },
  { P3, P7, P6, P2 },
  { P4, P1, P5, P8 },
};