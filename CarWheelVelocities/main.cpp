//
//  main.cpp
//  CarWheelVelocities
//
//  Created by tab on 4/23/18.
//  Copyright © 2018 tab. All rights reserved.
//

#include <iostream>


#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <list>
#include <unordered_map>

#include "TrackerHough.hpp"
#include "Wheel.hpp"
#include "Constants.h"



using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    //open the video file
    VideoCapture reader("./res/cars_passing_input.mp4");
    int ex = static_cast<int>(reader.get(CV_CAP_PROP_FOURCC));
    int frameCount = reader.get(CAP_PROP_FRAME_COUNT);
    int WIDTH = reader.get(CAP_PROP_FRAME_WIDTH);
    int HEIGHT = reader.get(CAP_PROP_FRAME_HEIGHT);
    int FPS = reader.get(CV_CAP_PROP_FPS);
    //init an VideoWriter to output processed video
    VideoWriter outputVideo("./res/output.mp4", ex, FPS/3, Size(WIDTH,HEIGHT), true);
    // if cannot open the file
    if (!reader.isOpened()) {
        cout << "cannot open video file" << endl;
        return EXIT_FAILURE;
    }
    
    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write"<< endl;
        return EXIT_FAILURE;
    }
    
    
    
//    reader.set(1, 72);
    int count = 0;
    Mat frame;
    
    //    set parameters for tracker
    TrackerHough::Params params;
    params.param1 = HOUGH_CIRCLE_PARA1;
    params.param2 = HOUGH_CIRCLE_PARA2;
    params.minRadius = MIN_WHEEL_R;
    params.maxRadius = MAX_WHEEL_R;
    params.width = WIDTH;
    
    //    create tracker
    Ptr<TrackerHough> tracker = TrackerHough::create(params);
    
    //    use a list to record wheels
    std::vector<Wheel> wheelList = {};
    
    //    process the image frame by frame
    while (true) {
        cout<<"frame count "<<count<<endl;
        count += 1;
        reader >> frame; // get a new frame from camera
        Mat imageGray,imageBlur;
        Mat imageCrop = frame(Rect(0,300,WIDTH,400));
        //    convert to gray
        if(imageCrop.channels() > 1)
        {
            cvtColor( imageCrop, imageGray, CV_BGR2GRAY );
        }
        else
        {
            imageGray = imageCrop.clone();
        }
        
        //apply Gaussian Blur before Hough Circle detect
        GaussianBlur(imageGray, imageBlur, Size(10*SIGMA-1,10*SIGMA-1), SIGMA, SIGMA);
        
        //update tracking wheelList with the blured image in each frame
        tracker->update(imageBlur, wheelList);
        
        //draw and show output image
        std::vector<Wheel>::iterator iter = wheelList.begin();
        for (iter = wheelList.begin(); iter!=wheelList.end();) {
            float circleX = iter->circle[0];
            float circleY = iter->circle[1]+300;
            float circleR = iter->circle[2];
            Point center(circleX, circleY);
            Point centerID(circleX, circleY-circleR);
            circle(frame, center, circleR, Scalar(0, 255, 0), 5);
            putText(frame, to_string(iter->index), centerID, 1, 10, Scalar(255, 0, 0),3);
            //if the velocity has mutilple records, show the average speed
            if (iter->xPosVec.size()>=2) {
                std::ostringstream speedText;
                float xLastFrame = iter->xPosVec[0];
                float dis = circleX-xLastFrame;
                float speed = dis/iter->xPosVec.size();
                speedText<<speed<<" pix/frame";
                Point textOrg(circleX, circleY+circleR);
                putText(frame, speedText.str(), textOrg, 1, 5, Scalar(0, 255, 0),3);
            }
            //if the wheel reach the edge of the image, remove it from the tracking list
            if (circleX>WIDTH-MARGIN_DIST) {
                iter = wheelList.erase(iter);
            }else{
                ++iter;
            }
        }
        imshow("Tracking", frame);
        std::ostringstream originalName;
        originalName << "./res/frame_" << count << ".jpg";
//        imwrite(originalName.str(), frame);
        outputVideo << frame;
        cv::waitKey(1);
        if(count == frameCount-1)
            break;
    }
    outputVideo.release();
    reader.release();
    return 0;
}
