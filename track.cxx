#include "track.h"
#include "img_template.tpl"

const double Track::MHI_DURATION = 1;
const double Track::MAX_TIME_DELTA = 0.5;
const double Track::MIN_TIME_DELTA = 0.05;
const int Track::FBSIZE = 4;

Track::Track(){
  last = 0;
  diff_threshold = 30;
}

Track::~Track(){
}

void Track::update(IplImage *img){
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

CvSeq* Track::segments(){
  return segs;
}
