#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _SATORI_APP_H_
#define _SATORI_APP_H_

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

// namespaces
using namespace std;

class SatoriApp{
public:
  // Constructors
  SatoriApp();	 // constructor 
  ~SatoriApp(); // destructor

  // Access Functions
  int get_size();		// returns number of processed images
    
  // Action Functions
  bool add(string);			// add an image
  int run();				// assumes DEFAULT_VERBOSITY
  int run(bool);			// run application
  void animate(string);			// assumes DEFAULT_VERBOSITY
  void animate(string, bool);		// output a movie of the results
  int run_webcam(bool verbose);
    
private:
  // Data representation objects
  vector<IplImage*> orig_images;
  vector<IplImage*> gray_images;
  vector<IplImage*> annotated_images;

  // State of machine
  bool ran;				// whether differences have been calculated
  char *where_flow_found;
  int num_tracked_points;
  int lk_flags;
  
  // Flow machine
  Flow flow;

  // Images
  IplImage *swap_img;

  // Points to track
  // CvPoint2D32f *prev_points, *curr_points, *swap_points;

  // Pyramids
  // IplImage *prev_pyramid, *curr_pyramid, *swap_pyramid;

  // Action Functions
  IplImage* annotate_img(IplImage*);
};

#endif
