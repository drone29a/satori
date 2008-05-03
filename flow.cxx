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
    where_flow_found = NULL;
    num_tracked_points = 0;
    lk_flags = 0;
    // Points to track
    prev_points = NULL;
    curr_points = NULL;
    swap_points = NULL;
}

Flow::~Flow(){
    // Destructor
    // release all images
    cvReleaseImage(&prev_pyramid);
    cvReleaseImage(&curr_pyramid);
    cvReleaseImage(&swap_pyramid);
}

// Action Functions
void Flow::init(IplImage *initial_img){

    // set up state of machine for new run
    prev_points = (CvPoint2D32f*)cvAlloc(MAX_POINTS_TO_TRACK*sizeof(prev_points[0]));	// initially NULL
    curr_points = (CvPoint2D32f*)cvAlloc(MAX_POINTS_TO_TRACK*sizeof(curr_points[0]));	// initially NULL
    where_flow_found = (char*)cvAlloc(MAX_POINTS_TO_TRACK);		// initially NULL

    // setup pyramids for flow
    prev_pyramid = cvCreateImage(cvGetSize(initial_img), IPL_DEPTH_8U, 1);	// initially NULL
    curr_pyramid = cvCreateImage(cvGetSize(initial_img), IPL_DEPTH_8U, 1);	// initially NULL

    // get initial set for feature detection
    IplImage* eig = cvCreateImage(cvGetSize(initial_img), 32, 1);
    IplImage* temp = cvCreateImage(cvGetSize(initial_img), 32, 1);
    double quality = 0.01;
    double min_distance = 5;
    num_tracked_points = MAX_POINTS_TO_TRACK;

    // detect number of features to track
    cvGoodFeaturesToTrack(initial_img, eig, temp, curr_points, &num_tracked_points, 
                          quality, min_distance, 0, 3, 0, 0.04 );
    cvFindCornerSubPix(initial_img, curr_points, num_tracked_points, 
                       cvSize(WINDOW_SIZE,WINDOW_SIZE), cvSize(-1,-1), 
                       cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

    // release temporary images
    cvReleaseImage(&eig);
    cvReleaseImage(&temp);
}

void Flow::pair_flow(IplImage* img1, IplImage* img2){
    // calculate flow and track points (modified Lucas & Kanade algorithm)
    cvCalcOpticalFlowPyrLK(img1, img2, 
                           prev_pyramid, curr_pyramid, 
                           prev_points, curr_points, num_tracked_points, 
                           cvSize(WINDOW_SIZE,WINDOW_SIZE), 3, where_flow_found, 
                           0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), lk_flags);

    lk_flags |= CV_LKFLOW_PYR_A_READY; // A pyramid will be ready > 1st time

    // update points
    CV_SWAP(prev_pyramid, curr_pyramid, swap_pyramid);
    CV_SWAP(prev_points, curr_points, swap_points);
}

int Flow::point_count(){
    return num_tracked_points;
}
