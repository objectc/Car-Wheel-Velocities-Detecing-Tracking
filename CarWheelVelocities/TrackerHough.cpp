//
//  HoughTracker.cpp
//  CarWheelVelocities
//
//  Created by tab on 4/26/18.
//  Copyright © 2018 tab. All rights reserved.
//

#include <opencv2/xfeatures2d/nonfree.hpp>

#include "TrackerHough.hpp"
#include "Constants.h"

TrackerHough::~TrackerHough()
{
}
TrackerHough::Params::Params(){}
bool TrackerHough::init( InputArray image, const std::vector<Wheel>& wheelList ){
    if( isInit )
    {
        return false;
    }
    
    if( image.empty() )
        return false;
    
    bool initTracker = initImpl( image.getMat(), wheelList);
    
    if( initTracker )
    {
        isInit = true;
    }
    
    return initTracker;
}


/**
 Update wheels data for each frame

 @param image input image
 @param wheelList the existing wheel list
 @return true or false */
bool TrackerHough::update(cv::InputArray image, std::vector<Wheel> &wheelList){
    if( !isInit )
    {
        return false;
    }
    
    if( image.empty() )
        return false;
    
    return updateImpl( image.getMat(), wheelList );
}

Ptr<TrackerHough> TrackerHough::create(const TrackerHough::Params &parameters)
{
    return Ptr<TrackerHough>(new TrackerHoughImpl(parameters));
}
Ptr<TrackerHough> TrackerHough::create()
{
    return Ptr<TrackerHough>(new TrackerHoughImpl());
}
//Ptr<TrackerHough> TrackerHough::createDetetor(const TrackerHough::Params &parameters)
//{
//    return Ptr<TrackerHough>(new DetectorWheel(parameters));
//}







TrackerHoughImpl::TrackerHoughImpl(const TrackerHough::Params &parameters): params( parameters ){
    isInit = true;
}

bool TrackerHoughImpl::initImpl(const cv::Mat &image, const std::vector<Wheel>& wheelList){
    return true;
}


/**
 The implementation of tracking update for each frame

 @param image input image
 @param wheelList the existing wheel list
 @return true or false
 */
bool TrackerHoughImpl::updateImpl(const cv::Mat &image, std::vector<Wheel>& wheelList){
    HoughCircles(image, candidates, CV_HOUGH_GRADIENT, 1, params.width/3, params.param1, params.param2, params.minRadius, params.maxRadius);
    std::vector<Wheel>::iterator iter;
    voteForCircles(image);
    for (iter = wheelList.begin(); iter!=wheelList.end(); ++iter) {
        iter->isTracked = false;
    }
    int cnt=0;
    for (auto circle : candidates) {
        if (voteVec[cnt]>15) {
            if (!updateNewWheel(circle, wheelList)) {
                updateTracking(circle, wheelList);
            }
        }
        cnt++;
    }
    return true;
}


/**
 Check is the circle should be the new track of the existing wheel

 @param circle the wheel candidate
 @param wheelList the existing wheel list
 @return true or false
 */
bool TrackerHoughImpl::updateTracking(const cv::Vec3f &circle, std::vector<Wheel> &wheelList ){
    if (isValidAsWheel(circle)) {
        std::vector<Wheel>::iterator iter;
        for (iter = wheelList.begin(); iter!=wheelList.end(); ++iter) {
//            only consider untracked wheel in current frame
            if (iter->isTracked) {
                continue;
            }
            float circleX = circle[0];
            float circleY = circle[1];
            float circleR = circle[2];
            float offsetX = circleX - iter->circle[0];
            if( offsetX>MIN_WHEEL_X_OFFSET_TRACKING &&
                offsetX<MAX_WHEEL_X_OFFSET_TRACKING &&
                abs(circleY-iter->circle[1])<MAX_WHEEL_Y_OFFSET &&
                abs(circleR-iter->circle[2])<MAX_WHEEL_R_DIFF
              ){
                iter->circle = circle;
                iter->xPosVec.push_back(circleX);
                iter->isTracked = true;
                return true;
            }
        }
    }
    return false;
}


/**
 Check is the circle should be a new wheel

 @param circle the wheel candidate
 @param wheelList the existing wheel list
 @return true or false
 */
bool TrackerHoughImpl::updateNewWheel(const cv::Vec3f &circle, std::vector<Wheel> &wheelList){
    if (isValidAsWheel(circle)) {
//        empty list just insert it
        float circleX = (circle)[0];
        if (wheelList.size() == 0) {
            Wheel newWheel(circle);
            newWheel.xPosVec.push_back(circleX);
            wheelList.push_back(newWheel);
            return true;
        }
        else {
//            need to compare the relation between existing wheels
            Vec3f lastWheelCircle = wheelList.back().circle;
            float lastWheelX = lastWheelCircle[0];
            if (lastWheelX-circleX>MIN_DIST_WHEELS) {
                Wheel newWheel(circle);
                newWheel.xPosVec.push_back(circleX);
                wheelList.push_back(newWheel);
                return true;
            }
        }
    }
    return false;
}


/**
 check the if the circle if valid or not

 @param circle the wheel candidate
 @return true or false
 */
bool TrackerHoughImpl::isValidAsWheel(const cv::Vec3f &circle){
    float circleX = circle[0];
    float circleR = circle[2];
    if (circleR>MIN_WHEEL_R && circleR<MAX_WHEEL_R &&
        circleX>MARGIN_TRACKING && circleX<params.width-MARGIN_TRACKING
        ) {
        return true;
    }
    return false;
}


/**
 Use SURF feature detection to vote for each wheel candidate

 @param image input Image
 */
void TrackerHoughImpl::voteForCircles(const cv::Mat &image){
    if (candidates.size()>0) {
        Ptr<xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create();
        surf->setHessianThreshold(2000);
        Mat des;
        std::vector<cv::KeyPoint>keyPts;
        surf->detect(image, keyPts);
        //        Mat kpImg;
        
        //        drawKeypoints(imageBlur, keyPts, kpImg, Scalar(255,0,0),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        //        imwrite("/Users/tab/Documents/Project/C++/CarWheelVelocities/CarWheelVelocities/res/sift.jpg", kpImg);
    }
    voteVec = std::vector<int>(candidates.size(),0);
    int cnt = 0;
    for (auto circle : candidates ) {
        
        float circleX = circle[0];
        float circleY = circle[1];
        float circleR = circle[2];
        Rect rect(circleX-circleR/2, circleY-circleR/2, circleR*2, circleR*2);
        for (auto kpt :keyPts) {
            if (rect.contains(kpt.pt)){
                voteVec[cnt]+=1;
	         }
        }
        cnt++;
    }
}
