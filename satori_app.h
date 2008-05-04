#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _SATORI_APP_H_
#define _SATORI_APP_H_

// includes
#include "common.h"
#include "flow.h"
#include "track.h"
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
  bool need_flow_init;
  bool need_track_init;
  char key_ch;
  bool do_flow;
  bool do_track;
  
  // Components
  Flow flow;
  Track track;

  // Images
  IplImage *swap_temp;

  // Pyramids
  IplImage *prev_pyramid, *pyramid;

  // Action Functions
  IplImage* annotate_img(IplImage*);
};

#endif
