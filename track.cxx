/*
 * track.cxx - Implementation of Track functions
 * (c) 2008 Michael Sullivan and Matt Revelle
 *
 * Last Revised: 05/04/08
 *
 * This program uses the Open Computer Vision Library (OpenCV)
 *
 */

#include "track.h"
#include "img_template.tpl"

const double Track::MHI_DURATION = 1;
const double Track::MAX_TIME_DELTA = 0.5;
const double Track::MIN_TIME_DELTA = 0.05;
const int Track::FBSIZE = 4;

Track::Track(){
  // init for motion segmentation
  last = 0;
  diff_threshold = 30;

  // init for camshift
  track_object = false;
  hsv = NULL;
  hdims = 16;
  vmin = 10;
  vmax = 256;
  smin = 30;
}

Track::~Track(){
}

void Track::update(IplImage *img){
  update_motion_segments(img);
  update_camshift(img);
}

void Track::update_motion_segments(IplImage *img){
  double timestamp = (double)clock()/CLOCKS_PER_SEC; // current time in seconds
  CvSize size = cvSize(img->width, img->height); // current frame size
  int index1 = last, index2;
  IplImage *silh;
    
  if (!mhi || mhi->width != size.width || mhi->height != size.height){
    if (buf == 0){
      buf = (IplImage**)malloc(FBSIZE*sizeof(buf[0]));
      memset(buf, 0, FBSIZE*sizeof(buf[0]));
    }

    // clear out buffers
    for (int i = 0; i < FBSIZE; ++i){
      cvReleaseImage(&buf[i]);
      buf[i] = cvCreateImage(size, IPL_DEPTH_8U, 1);
      cvZero(buf[i]);
    }
        
    cvReleaseImage(&mhi);
    cvReleaseImage(&segmask);

    mhi = cvCreateImage(size, IPL_DEPTH_32F, 1);
    cvZero(mhi);
    segmask = cvCreateImage(size, IPL_DEPTH_32F, 1);
  }

  // convert to grayscale
  cvCvtColor(img, buf[last], CV_BGR2GRAY);

  index2 = (last + 1) % FBSIZE;
  last = index2;

  silh = buf[index2];
  cvAbsDiff(buf[index1], buf[index2], silh); // get frame difference

  // threshold difference
  cvThreshold(silh, silh, diff_threshold, 1, CV_THRESH_BINARY);
  // update MHI
  cvUpdateMotionHistory(silh, mhi, timestamp, MHI_DURATION);

  if (!storage){
    storage = cvCreateMemStorage(0);
  }
  else {
    cvClearMemStorage(storage);
  }

  segs = cvSegmentMotion(mhi, segmask, storage, timestamp, MAX_TIME_DELTA);
}

void Track::update_camshift(IplImage *img){
  if (!hsv){
    hsv = cvCreateImage(cvGetSize(img), 8, 3);
    hue = cvCreateImage(cvGetSize(img), 8, 1);
    mask = cvCreateImage(cvGetSize(img), 8, 1);
    backproject = cvCreateImage(cvGetSize(img), 8, 1);
    float range[] = {0, 180};
    float *ranges = range;
    hist = cvCreateHist(1, &hdims, CV_HIST_ARRAY, &ranges, 1);
  }

  cvCvtColor(img, hsv, CV_BGR2HSV);

  if (track_object){
    int _vmin = vmin, _vmax = vmax;

    cvInRangeS(hsv, cvScalar(0, smin, MIN(_vmin,_vmax), 0),
                cvScalar(180, 256, MAX(_vmin, _vmax), 0), mask);
    cvSplit(hsv, hue, 0, 0, 0);

    cvCalcBackProject(&hue, backproject, hist);
    cvAnd(backproject, mask, backproject, 0);
    cvCamShift(backproject, track_window, 
               cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
               &track_comp, &_track_box);
    track_window = track_comp.rect;

    if (!img->origin)
      _track_box.angle = -_track_box.angle;
  }
}

CvSeq* Track::segments(){
  return segs;
}

const CvBox2D& Track::track_box() const{
  return _track_box;
}

void Track::select_window(CvRect& rect){
  CvRect comp_rect = (reinterpret_cast<CvConnectedComp*>(cvGetSeqElem(segs, 0)))->rect;
  rect = comp_rect;
}

void Track::reset(){
  init_camshift();
  track_object = true;
}

void Track::init_camshift(){
  int _vmin = vmin, _vmax = vmax;
  cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin, _vmax),0),
              cvScalar(180,256,MAX(_vmin, _vmax),0), mask );
  cvSplit( hsv, hue, 0, 0, 0 );
  
  float max_val = 0.f;
  select_window(track_window);
  cvSetImageROI(hue, track_window);
  cvSetImageROI(mask, track_window);
  cvCalcHist(&hue, hist, 0, mask);
  cvGetMinMaxHistValue(hist, 0, &max_val, 0, 0);
  cvConvertScale(hist->bins, hist->bins, max_val ? 255.0 / max_val : 0.0, 0);
  cvResetImageROI(hue);
  cvResetImageROI(mask);
  
  cout << "track_window: "
       << track_window.x << "  " << track_window.y << "  " 
       << track_window.width << "  " << track_window.height << endl;
}
