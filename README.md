# Car-Wheel-Velocities-Detection-and-Tracking

[Output Video in YouTube](https://www.youtube.com/watch?v=QiaXqL_SBwo)

[Output Files Download](https://drive.google.com/open?id=17CO01xQxgH23Pz5wD7wWunnRnkRIH4CF)

![Car](https://github.com/objectc/Car-Wheel-Velocities-Detection-and-Tracking/raw/master/car.jpg "Result")

1. Use Hough Circle Detector to detect circles. To make the results more accurate, I add some validation checking methods, such as the wheels' size and position restrictions. I also use SURF to extract features and measure how many features are in the circle candidate. The circles below a cut-off threshold will be eliminated.

2. I have tried several tracking algorithms provided by OpenCV such as TLD, CamShift. But the result was not good enough for wheel tracking. So I just implement my own tracking method based on the Hough Circle Detector. Each time a valid circle is detected, I'll compare it with the exsiting wheels with some conditions and determine if it is the same wheels of the next frame. As the output video shows, the accuracy seems to be quite well. 
