# 3dRotation
A rotating 3d object on an arduino UNO with a SSD1306 OLED display.

![alt text](/assets/images/Capture.gif) 

Simulation https://wokwi.com/projects/329034737849991764

Dimensions of the 3d object:

![alt text](/assets/images/Object.svg)
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

To detect which polygons is visible all polygons (triangle or rectangle) are defined counterclockwise.
For example 
| rectangle | Order of points |
| --- | --- |
| 0 | P4, P3, P2, P1 |

![alt text](/assets/images/OrderRectangle0.svg)
