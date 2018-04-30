# Car-Wheel-Velocities-Detection-and-Tracking

[Output Video in YouTube](https://www.youtube.com/watch?v=QiaXqL_SBwo)

[Output Files Download](https://drive.google.com/open?id=17CO01xQxgH23Pz5wD7wWunnRnkRIH4CF)

![Car](https://github.com/objectc/Car-Wheel-Velocities-Detection-and-Tracking/raw/master/car.jpg "Result")

1. Convert the input image to grayscale, and then perform Gaussian Blur to filter out some noise. For the purpose of efficiency and accuracy, it's better only use part of the original image if you can estimate the wheels position first.
2. Use Canny edge detection to make use of the gradient magnitude and generate the image with only edges left.
3. Run Hough Circle Detector with the edges image as input to detect circles.
4. Check the circle. To make the results more accurate, I add some validation checking methods, such as the wheels' size and position restrictions. I also use SURF to extract features and measure how many features are in the circle candidate. The circles whose number of contained features below a cut-off threshold will be eliminated.
5. For the valid circles, compare them with the exsiting wheels with some conditions and determine if it is a new wheel or one of the existing  wheels in the next frame (tracking). For tracking, I also have tried several tracking algorithms provided by OpenCV, such as TLD, CamShift. But none of the result was good enough for wheel tracking. So I just implement my own tracking method based on the Hough Circle Detector. As the output video shows, the accuracy seems to be quite well.
