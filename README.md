# 3dRotation
A rotating 3d object on an arduino UNO/NANO with a SSD1306 OLED 128x64 pixel display.

![captured display sequence](/assets/images/Capture.gif) 

Simulation https://wokwi.com/projects/329034737849991764

Video https://www.youtube.com/watch?v=705kT8A-B_I

## Used 3D-Object
![default 3d object](/assets/images/Object.svg)

## Points
| Point | X | Y | Z |
| --- | :---: | :---: | :---: |
| P1 | - 32 | 32 | 32 |
| P2 | 32 | 32 | 32 |
| P3 | 32 | -32 | 32 |
| P4 | - 32 | -32 | 32 |
| P5 | - 32 | 32 | -32 |
| P6 | 32 | 32 | -32 |
| P7 | 32 | -32 | -32 |
| P8 | - 32 | -32 | -32 |
| P9 | 0 | 96 | 0 |
| P10 | 0 | -96 | 0 |

## Inner/Outer
To define which side of a polygon is inner or outer, all polygons (triangles or quadrangles) are ordered counterclockwise. These information will be used to detect which polygon is visible and needs to be drawn on screen or not. 

For example: The order for the polygon, spanned by points P1 to P4, is **P4, P3, P2, P1**

![ordered quadrangles](/assets/images/OrderedQuadrangle.svg)

## Triangles:
| Nr | Ordered points |
| --- | --- |
| 0 | P1, P2, P9 |
| 1 | P6, P5, P9 |
| 2 | P2, P6, P9 |
| 3 | P5, P1, P9 |
| 4 | P3, P4, P10 |
| 5 | P4, P8, P10 |
| 6 | P8, P7, P10 |
| 7 | P7, P3, P10 |

## Quadrangles:
| Nr | Ordered points |
| --- | --- |
| 0 | P4, P3, P2, P1 |
| 1 | P5, P6, P7, P8 |
| 2 | P2, P3, P7, P6 |
| 3 | P4, P1, P5, P8 |

## Hide backsides
To detect which polygon is visible and needs to be drawn on screen or not, the order of the points spanning the polygon is checked. If the points are ordered  counterclockwise, the polygon will be drawn. If the order is clockwise, the polygon is invisible and will not be drawn.

The following figure shows an example (Green = counterclockwise => Visible, frontside, Red = clockwise => Invisible, backside). To keep it simple, only the orders for the quadrangles are marked in the figure: 

![detect backsides](/assets/images/DetectBacksides.svg)

## Overlapping objects
For simple convex objects like the default 3d object in this project, which has no overlapping parts, drawing only the outer lines without filling the polygons is sufficient and the order in which the polygons are rendered does not matter.  

When multiple objects overlap or an object overlaps itself, we need to fill the polygons (as we have only black and white, we fill with black) and the order in which the polygons are rendered plays an important role. Since only 2KByte RAM is available on an ATMEGA328 microcontroller, a z-buffer is not an option. To allow at least simple overlapping objects, all polygons are sorted by their minimal z-values and displayed by increasing z-values. Glitches are possible due large polygons or perspective distortion.

Ordered display list:
| Display order position | Polygon type | Nr | Maximal z-value | Minimal z-value |
| ---: | --- | :---: | :---: | :---: |
| 0 | Quadrangle | 1 | -32 | -32 |
| 1 | Triangle | 1 | 0 | -32 |
| 2 | Triangle | 6 | 0 | -32 |
| 3 | Triangle | 2 | 32 | -32 |
| 4 | Triangle | 3 | 32 | -32 |
| 5 | Triangle | 5 | 32 | -32 |
| 6 | Triangle | 7 | 32 | -32 |
| 7 | Quadrangle | 2 | 32 | -32 |
| 8 | Quadrangle | 3 | 32 | -32 |
| 9 | Triangle | 0 | 32 | 0 |
| 10 | Triangle | 4 | 32 | 0 |
| 11 | Quadrangle | 0 | 32 | 32 |

Here is an example for a wrong display order, which can not be solved by z-value sorting. To solve this problem, the large side-quadrangles need to be split into smaller polygons.

![glitch example](/assets/images/glitch.png) 
