# 3dRotation
A rotating 3d object on an arduino UNO with a SSD1306 OLED display.

![alt text](/assets/images/Capture.gif) 

Simulation https://wokwi.com/projects/329034737849991764

## 3D-Object
Dimensions of the 3d object:

![alt text](/assets/images/Object.svg)
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
To define which side of a polygon is inner or outer all polygons (triangles or rectangles) are defined counterclockwise. These information will be used to detect which polygon is visible and needs to be drawn or not. 

For example: The order for polygon spanned by points P1 to P4 is **P4, P3, P2, P1**

![alt text](/assets/images/OrderRectangle0.svg)

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

## Rectangles:
| Nr | Ordered points |
| --- | --- |
| 0 | P4, P3, P2, P1 |
| 1 | P5, P6, P7, P8 |
| 2 | P2, P3, P7, P6 |
| 3 | P4, P1, P5, P8 |

## Hide backsides
To detect which polygon is visible and needs to be drawn or not, the alignment of the points spanning the polygon is used. If the points are aligned in counterclockwise, the polygon will be drawn. If the alignment is counerclockwise, the polygon is invisible and will not be drawn.
The following figure shows an example for the alignments (Green = counterclockwise => Visible, Red = clockwise => Invisible). To keep it simple, only the alingments for the rectangles are shown. 
![alt text](/assets/images/DetectBacksides.svg)
