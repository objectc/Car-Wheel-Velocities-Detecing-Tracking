//
//  VideoReader.cpp
//  CarWheelVelocities
//
//  Created by tab on 4/23/18.
//  Copyright © 2018 tab. All rights reserved.
//

#include "VideoReader.hpp"
#include <iostream>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "CamShift.h"

using namespace std;
using namespace cv;
using namespace camShift;
static const int SIGMA = 2;

void performCamShift(Rect trackWindow[]){

}

VideoReader::VideoReader(std::string const& path){
    //open the video file
    VideoCapture reader(path);
    camShift::CamShift camShift;
    // if cannot open the file
    if (!reader.isOpened()) {
        cout << "cannot open video file" << endl;
        return;
    }
    
    int frameCount = reader.get(CAP_PROP_FRAME_COUNT);
    int count = 0;
    bool isWheelsDetected = false;
    Rect roi[2],trackWindow[2];
    
    float hranges[] = {0,180};
    const float* phranges = hranges;
    int hsize = 16;
    Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
    while(true)
    {
        cout<<"frame count "<<count<<endl;
        count += 1;
        reader >> frame; // get a new frame from camera
        cvtColor(frame, hsv, CV_BGR2HSV);
        hue.create(hsv.size(), hsv.depth());
        int ch[] = {0, 0};
        mixChannels(&hsv, 1, &hue, 1, ch, 1);
        if (!isWheelsDetected) {
            std::ostringstream originalName;
            originalName << "/Users/tab/Documents/Project/C++/CarWheelVelocities/CarWheelVelocities/res/original_" << count << ".jpg";
            imwrite(originalName.str(), frame);
            Mat blur,edges;
            cvtColor(frame, blur, CV_RGB2GRAY);
            //        Apply Gaussina Blur before Canny
            GaussianBlur(blur, blur, Size(6*SIGMA-1,6*SIGMA-1), SIGMA, SIGMA);
            //        Finds edges in an image using the Canny algorithm
            Canny(blur, edges, 50, 100);
            std::ostringstream edgeName;
            edgeName << "/Users/tab/Documents/Project/C++/CarWheelVelocities/CarWheelVelocities/res/edge_" << count << ".jpg";
                    imwrite(edgeName.str(), edges);
            isWheelsDetected = detect(blur);
            if (isWheelsDetected) {
                for (int wheelCnt = 0; wheelCnt<wheels.size(); ++wheelCnt){
                    float circleCenterX = wheels[wheelCnt][0];
                    float circleCenterY = wheels[wheelCnt][1];
                    float circleCenterR = wheels[wheelCnt][2];
                    roi[wheelCnt] = Rect(circleCenterX-circleCenterR/2, circleCenterY-circleCenterR/2, circleCenterR, circleCenterR);
                    

                    inRange(hsv, Scalar(0, 60, 32),
                            Scalar(180, 256, 256), mask);
                    Mat selection(hue, roi[wheelCnt]), maskroi(mask, roi[wheelCnt]);
                    calcHist(&selection, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                    normalize(hist, hist, 0, 255, NORM_MINMAX);
                    histimg = Scalar::all(0);
                    int binW = histimg.cols / hsize;
                    Mat buf(1, hsize, CV_8UC3);
                    for( int i = 0; i < hsize; i++ )
                        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                    cvtColor(buf, buf, COLOR_HSV2BGR);
                    for( int i = 0; i < hsize; i++ )
                    {
                        int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                        rectangle( histimg, Point(i*binW,histimg.rows),
                                  Point((i+1)*binW,histimg.rows - val),
                                  Scalar(buf.at<Vec3b>(i)), -1, 8 );
                    }
                    trackWindow[wheelCnt] = roi[wheelCnt];
                    
                }
                for (int wheelCnt = 0; wheelCnt<wheels.size(); ++wheelCnt){
                    
                }
            }
        }
            if (trackWindow[0].x + trackWindow[0].width>=frame.cols) {
                break;
            }
            for (int wheelCnt = 0; wheelCnt<wheels.size(); ++wheelCnt){
                // Perform CAMShift
                calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
                backproj &= mask;
                cout<<"trackWindow"<<trackWindow[wheelCnt]<<endl;
                RotatedRect trackBox = CamShift(backproj, trackWindow[wheelCnt],
                                                TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
                cout<<"trackWindow"<<trackBox.boundingRect()<<endl;
                trackWindow[wheelCnt] = trackBox.boundingRect();
                if( trackWindow[wheelCnt].area() <= 1 )
                {
                    int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
                    trackWindow[wheelCnt] = Rect(trackWindow[wheelCnt].x - r, trackWindow[wheelCnt].y - r,
                                                 trackWindow[wheelCnt].x + r, trackWindow[wheelCnt].y + r) &
                    Rect(0, 0, cols, rows);
                }
                circle(frame, trackBox.center, trackBox.size.width, Scalar(0, 255, 0), 5);
                std::ostringstream originalName;
                originalName << "/Users/tab/Documents/Project/C++/CarWheelVelocities/CarWheelVelocities/res/original_" << count << ".jpg";
                imwrite(originalName.str(), frame);
//                if( true )
//                    cvtColor( backproj, image, COLOR_GRAY2BGR );
//                ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );
            }
//        if(count == frameCount-1)
        if(count == 60)
            break;
    }
    reader.release();
}


    /*
    // the camera will be deinitialized automatically in VideoCapture destructor
//    return 0;
    
    int intCodecType = static_cast<int>(reader.get(CAP_PROP_FOURCC));
    
    char charArrayCodecType[] = { (char)(intCodecType & 0XFF) , (char)((intCodecType & 0XFF00) >> 8),(char)((intCodecType & 0XFF0000) >> 16),(char)((intCodecType & 0XFF000000) >> 24), 0 };
    
    
    
    //VideoWriter videoWriter("output_video.avi", reader.get(CV_CAP_PROP_FOURCC), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.mp4", reader.get(CV_CAP_PROP_FOURCC), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    VideoReader.
    VideoWriter videoWriter("output_vid.mp4", CV_FOURCC('M','P','4','V'), reader.get(CAP_PROP_FPS), Size(reader.get(CAP_PROP_FRAME_WIDTH), reader.get(CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.avi", CV_FOURCC('X','V','I','D'), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.avi", CV_FOURCC('A', 'V', 'C', '1'), 25, Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.mp4", CV_FOURCC('m', 'p', '4', 'v'), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.mp4", CV_FOURCC('M', 'J', 'P', 'G'), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    //VideoWriter videoWriter("output_video.avi", CV_FOURCC('M', 'J', 'P', 'G'), reader.get(CV_CAP_PROP_FPS), Size(reader.get(CV_CAP_PROP_FRAME_WIDTH), reader.get(CV_CAP_PROP_FRAME_HEIGHT)));
    
    if (!videoWriter.isOpened()) {
        std::cout << "error instantiating VideoWriter" << "\n\n";
        _getch();                   // it may be necessary to change or remove this line if not using Windows
        return(0);
    }
    
    
    Mat imgScene;
    reader.read(imgScene);
    
    char chCheckForEscKey = 0;
    
    while (reader.isOpened() && chCheckForEscKey != 27) {
        
        // show the current frame
        imshow("imgScene", imgScene);
        
        /*
         // convert to grayscale
         Mat imgGrayscale;
         cvtColor(imgScene, imgGrayscale, CV_BGR2GRAY);
         // get an Otsu thresh value to use for Canny low/high params
         Mat imgDummy;
         double otsuThreshVal = threshold(imgGrayscale, imgDummy, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
         // find canny edges
         Mat imgCanny;
         Canny(imgGrayscale, imgCanny, otsuThreshVal / 2.0, otsuThreshVal);
         imshow("imgCanny", imgCanny);
         chCheckForEscKey = waitKey(1);
         */
        /*
        Mat imgOutput;
        flip(imgScene, imgOutput, 1);
        imshow("imgOutput", imgOutput);
        
        // write the Canny edge frame to the output video
        videoWriter.write(imgOutput);
        
        // if there is another frame get it, else show end of video message and bail
        if ((reader.get(CV_CAP_PROP_POS_FRAMES) + 1) < reader.get(CV_CAP_PROP_FRAME_COUNT)) {
            reader.read(imgScene);
        }
        else {
            std::cout << "end of video\n";
            break;
        }
        
        chCheckForEscKey = waitKey(1);
        
    }
    
    //if the user did not press esc (i.e. we reached the end of the video) hold the windows open to allow the "end of video" message to show
    if (chCheckForEscKey != 27) {
        waitKey(0);
    }
    // note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows
    
    reader.release();
    videoWriter.release();
*/
VideoReader::~VideoReader(){
        
}
bool VideoReader::detect(Mat const& edges){
    //use hough transform to detect wheels(wheels)
    //the radius is between 60-80
    //the distance between two wheels should be set
    HoughCircles(edges, wheels, CV_HOUGH_GRADIENT, 1, edges.cols/3, 100, 40, 60, 80);
    
    std::ofstream output("./wheel.txt");
    
    //
    if(wheels.size() == 0) return false;
    //Loop over all detected wheels and outline them on the original image
    for(size_t current_circle = 0; current_circle < wheels.size(); ++current_circle) {
        Point center(round(wheels[current_circle][0]), round(wheels[current_circle][1]));
        int radius = round(wheels[current_circle][2]);
        output<<center;
//        circle(carImage, center, radius, Scalar(0, 255, 0), 5);
    }
    output.close();
    return true;
}
