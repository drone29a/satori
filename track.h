/*
 * track.h - Segment Motion and Track with CAMSHIFT
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

#ifndef _TRACK_H_
#define _TRACK_H_

// includes
#include "common.h"
#include "flow.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

// namespace preparation
using namespace std;

class Track{
  static const double MHI_DURATION;
  static const double MAX_TIME_DELTA;
  static const double MIN_TIME_DELTA;
  static const int FBSIZE;

 public:
  Track();
  ~Track();
  
  void update(IplImage*); // update the motion segments        
  void reset(); // reset to largest segment
  void reset(Flow&);
  CvSeq* segments(); // return found motion segments
  const CvConnectedComp* largest_segment();
  const CvBox2D& track_box() const; // return ref to tracked area

 private:
  // variables for motion segmentation
  IplImage **buf;
  int last;
  int diff_threshold;
  IplImage *mhi;
  IplImage *segmask; // motion segmentation map
  CvMemStorage* storage; // temp storage
  CvSeq *segs;
  bool segs_sorted;

  // variables for camshift
  IplImage *hsv, *hue, *backproject, *mask, *tmp1, *tmp2, *tmp3;
  CvHistogram *hist;
  CvBox2D _track_box;
  CvConnectedComp track_comp;
  int hdims;
  int vmin, vmax, smin;
  bool track_object;
  CvRect track_window;

  // methods
  void update_motion_segments(IplImage*);
  void update_camshift(IplImage*);
  void select_window(CvRect&);
  void select_window(CvRect&, Flow&);
  void init_camshift();
};

#endif
