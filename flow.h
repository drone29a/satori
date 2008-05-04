/*
 * flow.h - Calculate Optical Flows and Differences
 * (c) 2008 Michael Sullivan and Matt Revelle
 *
 * Last Revised: 05/04/08
 *
 *  This program uses the Open Computer Vision Library (OpenCV)
 *
 */

#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _FLOW_H_
#define _FLOW_H_

// includes
#include "common.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

// namespace preparation
using namespace std;

// types
class Flow{
 public:
    // Constructors
    Flow();	 // constructor 
    ~Flow(); // destructor

    // Access Functions
    int point_count();
    
    // Action Functions
    void init(IplImage*);
    void pair_flow(IplImage* prev, IplImage* prev_pyr,
                   IplImage* curr, IplImage* curr_pyr);	// calculate the flow between two images
    
    // Current points tracked
    CvPoint2D32f *points;
 private:
    // State of machine
    bool ran;				// whether differences have been calculated
    char* flow_pixels;
    int _point_count;
    int lk_flags;

    // Points to track
    CvPoint2D32f *prev_points, *swap_points;
};

#endif
