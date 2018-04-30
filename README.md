# Car-Wheel-Velocities-Detection-and-Tracking
![]()
1. Use Hough Circle Detector to detect circles. To make the results more accurate, I add some validation checking methods, such as some assumptions such as the range of the wheels' size and position. I also use SURF to extract features and measure how many features are in the circle candidate. The circles below a cut-off threshold will be eliminated.
2. I have tried the tracking algorithms in OpenCV such as TLD, CamShift. But the result was not good enough. So I just implement my own tracking method based on the Hough Circle Detector.

