#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _TRACK_H_
#define _TRACK_H_

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

class Track{
  static const double MHI_DURATION;
  static const double MAX_TIME_DELTA;
  static const double MIN_TIME_DELTA;
  static const int FBSIZE;

 public:
  Track();
  ~Track();

  void update(IplImage* img); // update the motion segments        
  CvSeq* segments(); // return found motion segments

 private:
  IplImage **buf;
  int last;
  int diff_threshold;

  IplImage *mhi;
  IplImage *segmask; // motion segmentation map
  CvMemStorage* storage; // temp storage
  CvSeq *segs;
};

#endif
