//
//  Wheel.cpp
//  CarWheelVelocities
//
//  Created by tab on 4/27/18.
//  Copyright © 2018 tab. All rights reserved.
//

#include "Wheel.hpp"
int Wheel::id = 0;
Wheel::Wheel(const cv::Vec3f & circle):circle(circle),isTracked(false),index(id++){
}
