/*
 * satori_app.cxx - Satori Application
 * (c) 2008 Michael Sullivan and Matt Revelle
 *
 * Last Revised: 05/04/08
 *
 *  This program uses the Open Computer Vision Library (OpenCV)
 *
 */

#include "satori_app.h" 	// flow header file
#include "img_template.tpl"	// provides efficient access to pixels

// Constructors

SatoriApp::SatoriApp(){
  // Default Constructor

  // set up state of machine
  ran = false;  
    
  // do any components need init in run loop
  need_flow_init = false;
  need_track_init = true;

  // which components should be executed in run loop
  do_flow = false;
  do_track = false;

  // set pyramids to NULL in order to avoid destructor ugliness
  prev_pyramid = NULL;
  pyramid = NULL;
}

SatoriApp::~SatoriApp(){
  // Destructor
  // release all images

  for(int i = 0; i < orig_images.size(); i++){
    cvReleaseImage(&orig_images[i]);   
  }
  for(int j = 0; j < gray_images.size(); j++){
    cvReleaseImage(&gray_images[j]);
  }
  for(int k = 0; k < annotated_images.size(); k++){
    cvReleaseImage(&annotated_images[k]);
  }

  if(prev_pyramid) cvReleaseImage(&prev_pyramid);
  if(pyramid) cvReleaseImage(&pyramid);
}

// Access Functions

int SatoriApp::get_size(){
  return orig_images.size();
}

// Action Functions

bool SatoriApp::add(string filename){
  
  // load the image
  IplImage *img = NULL, *gray = NULL;
  img = cvLoadImage(filename.c_str(),1);  // scan in color
  if(!img) return false;

  // store the image
  orig_images.push_back(img);

  // convert to grayscale
  gray = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
  cvCvtColor(img, gray, CV_BGR2GRAY);

  // store the gray image
  gray_images.push_back(gray);

  return true;
}

int SatoriApp::run(){
  // Default run function (with default verbosity)
  return run(DEFAULT_VERBOSITY);
}

int SatoriApp::run_webcam(bool verbose){
  // capture input from webcam

  CvCapture* capture = 0;
  capture = cvCaptureFromCAM(0);  // capture from default device
  if(!capture){
    return -1;
  }

  cvNamedWindow("Webcam_Capture", 0 );

  IplImage *image = NULL, *prev_grey = NULL, *grey = NULL, *ann_image = NULL;

  for(;;){
  
    IplImage* frame = NULL;
  
    frame = cvQueryFrame(capture);
    if( !frame )
      break;

    if(!image){	// initialize data structures the first time
      image = cvCreateImage(cvGetSize(frame), 8, 3);
      image->origin = frame->origin;
      grey = cvCreateImage(cvGetSize(frame), 8, 1);
      prev_grey = cvCreateImage(cvGetSize(frame), 8, 1);
      pyramid = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);	// initially NULL
      prev_pyramid = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);  // initially NULL
    }

    cvCopy(frame, image, 0);
    cvCvtColor(image, grey, CV_BGR2GRAY);

    // perform operations
    if (do_flow && flow.point_count() > 0){
      // update pairs with flow information
      flow.pair_flow(prev_grey, prev_pyramid, grey, pyramid);
    }

    if (do_track){
      // track largest moving object
      track.update(image);
    }
    
    // prepare for next captured picture
    CV_SWAP(prev_grey, grey, swap_temp);
    CV_SWAP(prev_pyramid, pyramid, swap_temp);

    // display webcam output        
    ann_image = annotate(image);
    cvShowImage("Webcam_Capture", ann_image);
    cvReleaseImage(&ann_image);

    // Handle keyboard input
    key_ch = cvWaitKey(10);
    if( (char)key_ch == 27 )  // ESC key
      break;
    switch( (char) key_ch )
      {
      case 'f':
        flow.init(grey);
        do_flow = !do_flow;
        break;
      case 't':
        do_track = !do_track;
        break;
      case 'r':
        track.reset();
        break;
      default:
        ;
      }
  }
        
  cvReleaseCapture(&capture);
    
  return 0;      
}

int SatoriApp::run(bool verbose){

  if(orig_images.size() == 0){
    cout << "  * " << "No images to process!" << endl;
    return NO_IMAGES;
  }

  if(verbose)
    cout << endl << "  * " << "Calculating optical flow for " << orig_images.size() - 1 << " pairs of images..." << endl;

  // used to store (grayscale) input images
  IplImage *img1 = NULL, *img2 = NULL, *disp_image = NULL;

  // initialize for first frame
  img1 = cvCreateImage(cvGetSize(gray_images[0]), 8, 1);
  img1->origin = gray_images[0]->origin;
  // acquire usable image, grayscale it
  cvCopy(gray_images[0], img1, 0);

  // run flow algorithm on all remaining images
  for(int i = 0; i < orig_images.size() - 1; i++){
    
    // load images
    if(i == 0)	// necessary?
      img1 = cvCreateImage(cvGetSize(gray_images[i]), 8, 1);
    else
      img1 = cvCloneImage(gray_images[i]);
    img2 = cvCloneImage(gray_images[i+1]);
    
    // check that images are consistent
    int height1,width1,channels1;			
    int height2,width2,channels2;

    // parse first image data
    height1    = img1->height;
    width1     = img1->width;
    channels1  = img1->nChannels;
  
    // parse second image data
    height2    = img2->height;
    width2     = img2->width;
    channels2  = img2->nChannels;
 
    // check for inequality in image dimensions, channels
    if(!img1 || !img2 || height1 != height2 || width1 != width2 || channels1 != channels2){
      printf("[ERROR] Images are not same dimensions, number of channels!");
      return IMAGE_CONSISTENCY_FAILED;
    }

    // calculate flow between the two images (results modify private global variables)
    if(verbose)
      cout << "    * " << "Processing optical flow of image pair #" << i << "...";
    //        flow.pair_flow(img1, img2);

    // annotate resulting image
    annotated_images.push_back(annotate(orig_images[i+1]));	// animate colored second pair

    if(verbose)
      cout << "\t\t\t\t[OK]" << endl;
  }

  // mark as run
  ran = true;

  return 0;
}

void SatoriApp::animate(string outfolder){
  // Default animate function (with default verbosity)
  return animate(outfolder, DEFAULT_VERBOSITY);
}

void SatoriApp::animate(string outfolder, bool verbose){

  // make sure state of machine is ready for animation
  if(!ran){
    cout << "  * " << "Optical flow processing has not been performed!" << endl;
    return;
  }
  else if(annotated_images.size() != orig_images.size() - 1){
    cout << "  * " << "Not all images have been annotated!" << endl;
    return;
  }

  if(verbose)
    cout << endl << "  * " << "Outputing animation for " << orig_images.size() - 1 << " pairs of images..." << endl;

  for(int i = 0; i < orig_images.size() - 1; i++){

    if(verbose)
      cout << "    * " << "Animating image pair #" << i << "...";
    
    // output images
    stringstream ostream;
    string outfile;
    ostream << i << ".png";
    outfile = ostream.str();
    if(i < 10) outfile = "0" + outfile;
    if(i < 100) outfile = "0" + outfile;
    outfile = outfolder + outfile;

    cvSaveImage(outfile.c_str(),annotated_images[i]);      // add the frame to a file   

    if(verbose)
      cout << "\t\t\t\t[OK]" << endl;
  }
}

IplImage* SatoriApp::annotate(IplImage* img){
  // annotate a copy of the image
  IplImage* ann = cvCloneImage(img);

  if (do_flow){
    ann = annotate_flow(ann);
  }
  
  if (do_track){
    ann = annotate_track(ann);
  }
  
  return ann;
}
    
IplImage* SatoriApp::annotate_flow(IplImage* img){
  // add circles for each tracked point
  int num_points = flow.point_count();

  for(int i = 0; i < num_points; ++i) {
    CvPoint pt = cvPointFrom32f(flow.points[i]);
    cvCircle(img, pt, 3, CV_RGB(0,255,0), -1, 8, 0);
  }
  
  return img;
}

IplImage* SatoriApp::annotate_track(IplImage* img){
  // add boxes for each motion segment
  CvSeq *segs = track.segments();
  CvRect comp_rect;
  CvScalar color;

  for(int i = 0; i < segs->total; ++i){
    comp_rect = (reinterpret_cast<CvConnectedComp*>(cvGetSeqElem(segs, i)))->rect;
    color = CV_RGB(255, 0, 0);
    
    cvRectangle(img, 
                cvPoint(comp_rect.x, comp_rect.y),
                cvPoint(comp_rect.x + comp_rect.width,
                        comp_rect.y + comp_rect.height),
                color);
  }

  return img;
}
