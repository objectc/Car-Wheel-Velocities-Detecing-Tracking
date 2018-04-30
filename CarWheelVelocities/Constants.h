//
//  Constants.h
//  CarWheelVelocities
//
//  Created by tab on 4/26/18.
//  Copyright © 2018 tab. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

// Sigma for GaussianBlur
static const int SIGMA = 2;
// track margin
static const float MARGIN_TRACKING = 200;
// image draw margin
static const float MARGIN_DIST = 400;
// the minimum distance between two wheels
static const float MIN_DIST_WHEELS = 350;
// the minimum and maximum offset in x for tracking
static const float MIN_WHEEL_X_OFFSET_TRACKING = 60;
static const float MAX_WHEEL_X_OFFSET_TRACKING = 500;
// the maximum offset in y for both detecting and tracking
static const float MAX_WHEEL_Y_OFFSET = 30;
// the maximum difference in radius for both detecting and tracking
static const float MAX_WHEEL_R_DIFF = 30;

static const float MIN_WHEEL_R = 40;
static const float MAX_WHEEL_R = 85;
// parameters for HoughCircles func
static const double HOUGH_CIRCLE_PARA1 = 100;
static const double HOUGH_CIRCLE_PARA2 = 30;

// SURF feature vote threshold
static const double SURF_VOTE_THRESHOLD = 15;

#endif /* Constants_h */
