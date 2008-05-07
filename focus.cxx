/*
 * focus.cxx - Implementation of Focus class
 * (c) 2008 Michael Sullivan and Matt Revelle
 *
 * Last Revised: 05/04/08
 *
 * This program uses the Open Computer Vision Library (OpenCV)
 *
 */

#include "focus.h"
#include "img_template.tpl"

Focus::Focus(){
}

Focus::~Focus(){
  cvReleaseImage(&poly_img);
  cvReleaseImage(&point_img);
  cvReleaseImage(&and_img);
}

void Focus::update(const CvBox2D* track_box, 
                   const CvConnectedComp* motion_seg,
                   CvPoint2D32f* feature_points,
                   int num_points,
                   const CvSize& frame_size_,
                   bool& changed){
  frame_size = frame_size_;
  changed = false;

  if (!poly_img) {
    poly_img = cvCreateImage(frame_size, 8, 1);
    point_img = cvCreateImage(frame_size, 8, 1);
    and_img = cvCreateImage(frame_size, 8, 1);
  }

  if (track_box && motion_seg && feature_points){

    // Check if CAMSHIFT box and motion segment largely intersect
    draw_box(track_box, poly_img, cvScalar(255));
    draw_comp(motion_seg, point_img, cvScalar(255));
    float inter_amt = intersect_amount(poly_img, point_img, and_img);
    CvRect camshift_rect = cvBoundingRect(poly_img);
    CvRect seg_rect = cvBoundingRect(point_img);
    float size_ratio = camshift_rect.width*camshift_rect.height / seg_rect.width*seg_rect.height;
    
    // Calculate feature density for both CAMSHIFTed box and motion segment
    float cam_density = density(track_box, feature_points, num_points);
    float seg_density = density(motion_seg, feature_points, num_points);
    cout << cam_density << " " << seg_density << endl;
    float density_ratio = seg_density / cam_density;
  
    // If density is significantly higher in motion_seg, change focus
    if (density_ratio > 1.5) {
      changed = true;
    }
  }
}

const CvConnectedComp& Focus::focus_area(){
  return last_focus_area;
}

float Focus::density(const CvBox2D* box, CvPoint2D32f* pts, int num_pts){
  CvPoint2D32f v_float[4];
  cvBoxPoints(*box, v_float);

  CvPoint v[4];
  for (int i = 0; i < 4; ++i){
    v[i] = cvPointFrom32f(v_float[i]);
  }
  
  float count = (float)intersect_count(v, 4, pts, num_pts);

  return count / (float)(box->size.width * box->size.height);
}

float Focus::density(const CvConnectedComp* comp, CvPoint2D32f* pts, int num_pts){
  CvPoint v[4];
  v[0] = cvPoint(comp->rect.x, comp->rect.y);
  v[1] = cvPoint(comp->rect.x + comp->rect.width, comp->rect.y);
  v[2] = cvPoint(comp->rect.x + comp->rect.width, comp->rect.y + comp->rect.height);
  v[3] = cvPoint(comp->rect.x, comp->rect.y + comp->rect.height);
  
  float count = (float)intersect_count(v, 4, pts, num_pts);
  return count / (float)(comp->rect.width * comp->rect.height);
}

int Focus::intersect_count(CvPoint* verts, int num_verts, 
                           CvPoint2D32f* pts, int num_pts){
  cvZero(poly_img);
  cvZero(point_img);
  cvZero(and_img);
  
  // Draw polygon
  cvFillConvexPoly(poly_img, verts, num_verts, cvScalar(255));
  
  // Draw points
  for (int i = 0; i < num_pts; ++i){
     cvRectangle(point_img, cvPointFrom32f(pts[i]), cvPointFrom32f(pts[i]),
                 cvScalar(255), CV_FILLED);
  }
  
  cvAnd(poly_img, point_img, and_img);

  int count = cvCountNonZero(and_img);

  return count;
}
