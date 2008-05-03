/*
 * calc_flows.cxx - Implementation of Calc_Flows functions
 * (c) 2008 Michael Sullivan
 *
 * Version 1.0.0
 * Last Revised: 04/29/08
 *
 * Version History:
 *  1.0.0 - Primary implementation (04/29/08)
 *
 *  This program utilizes the Open Computer Vision Library (OpenCV)
 *
 */

#include "flow.h" 	// flow header file
#include "img_template.tpl"	// provides efficient access to pixels

// Constructors

Flow::Flow(){
    // Default Constructor

    // set up state of machine
    flow_pixels = NULL;
    _point_count = 0;
    lk_flags = 0;

    // set up state of machine for new run
    prev_points = (CvPoint2D32f*)cvAlloc(MAX_POINTS_TO_TRACK*sizeof(prev_points[0]));	// initially NULL
    curr_points = (CvPoint2D32f*)cvAlloc(MAX_POINTS_TO_TRACK*sizeof(curr_points[0]));	// initially NULL
    flow_pixels = (char*)cvAlloc(MAX_POINTS_TO_TRACK);		// initially NULL
}

Flow::~Flow(){
    // Destructor
}

// Action Functions
void Flow::init(IplImage *initial_img){
    // get initial set for feature detection
    IplImage* eig = cvCreateImage(cvGetSize(initial_img), 32, 1);
    IplImage* temp = cvCreateImage(cvGetSize(initial_img), 32, 1);
    double quality = 0.01;
    double min_distance = 5;
    _point_count = MAX_POINTS_TO_TRACK;

    // detect number of features to track
    cvGoodFeaturesToTrack(initial_img, eig, temp, curr_points, &_point_count, 
                          quality, min_distance, 0, 3, 0, 0.04 );
    cvFindCornerSubPix(initial_img, curr_points, _point_count, 
                       cvSize(WINDOW_SIZE,WINDOW_SIZE), cvSize(-1,-1), 
                       cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

    // release temporary images
    cvReleaseImage(&eig);
    cvReleaseImage(&temp);
}

void Flow::pair_flow(IplImage* img1, IplImage* img1_pyr,
                     IplImage* img2, IplImage* img2_pyr){
    // calculate flow and track points (modified Lucas & Kanade algorithm)
    cvCalcOpticalFlowPyrLK(img1, img2, 
                           img1_pyr, img2_pyr, 
                           prev_points, curr_points, _point_count, 
                           cvSize(WINDOW_SIZE,WINDOW_SIZE), 3, flow_pixels, 
                           0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), lk_flags);

    lk_flags |= CV_LKFLOW_PYR_A_READY; // A pyramid will be ready > 1st time

    // update points
    CV_SWAP(prev_points, curr_points, swap_points);
}

int Flow::point_count(){
    return _point_count;
}
