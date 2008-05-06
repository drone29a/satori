/*
 * focus.h - Locate interesting motions
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

#ifndef _FOCUS_H_
#define _FOCUS_H_

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

class Focus{
  /* This class is used to identify and update the focus of the scene.
     There are two conditions that will cause a refocus:
       - The track box from CAMSHIFT has a low feature density
       - The largest motion segment has a significantly higher feature density
         than the track box

     Another option is to consider the historical feature density of the last motion segment
     used to seed CAMSHIFT and only reinit CAMSHIFT with a new motion segment if it does
     not intersect with the CAMSHIFT box and it has a larger feature density than the historical
     motion segment.
  */
 public:
  Focus();
  ~Focus();

  // methods
  void update(const CvBox2D* track_box, 
              const CvConnectedComp* motion_area,
              CvPoint2D32f* feature_points,
              int num_points,
              const CvSize& frame_size_, // size of frames
              bool& changed=false); // check if focus change is needed

  const CvConnectedComp& focus_area(); // the last focus area

 private:
  // variables
  CvConnectedComp last_focus_area;
  IplImage *poly_img; // used for finding feature intersection
  IplImage *point_img; // used for finding feature intersection
  IplImage *and_img; // used for finding feature intersection
  CvSize frame_size; // gets updated by calls to update

  // methods
  float density(const CvBox2D*, CvPoint2D32f*, int);
  float density(const CvConnectedComp*, CvPoint2D32f*, int);
  int intersect_count(CvPoint*, int, CvPoint2D32f*, int);
};

#endif
