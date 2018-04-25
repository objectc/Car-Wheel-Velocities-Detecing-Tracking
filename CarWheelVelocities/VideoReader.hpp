//
//  VideoReader.hpp
//  CarWheelVelocities
//
//  Created by tab on 4/23/18.
//  Copyright © 2018 tab. All rights reserved.
//

#ifndef VideoReader_hpp
#define VideoReader_hpp

#include <stdio.h>

#include <string>
#include <unordered_map>
#include <vector>

#include <opencv2/opencv.hpp>

class VideoReader{
private:
    //use map to store wheels with frame as the key
//    std::unordered_map<int, std::vector<cv::Vec3f>>wheelsMap;
    //
    std::vector<cv::Vec3f> wheels;
    std::vector<float>speedArray;
    
    
    /**
     Detect the vehicle's wheels

     @param edges input image to detect
     @return No wheels detected will return false, otherwise return true
     */
    bool detect(cv::Mat const& edges);
    
public:
    VideoReader(std::string const& path);
    ~VideoReader();
    
    /**
     process the input video frame by frame
     */
    void processVideo();
};

#endif /* VideoReader_hpp */
