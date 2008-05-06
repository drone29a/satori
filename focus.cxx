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
                   const CvConnectedComp* motion_area,
                   CvPoint2D32f* feature_points,
                   const CvSize& frame_size_,
                   bool& changed){
  if (!poly_img) {
    poly_img = cvCreateImage(frame_size, 8, 1);
    point_img = cvCreateImage(frame_size, 8, 1);
    and_img = cvCreateImage(frame_size, 8, 1);
  }

  frame_size = frame_size_;
  changed = false;
}

const CvConnectedComp& Focus::focus_area(){
  return last_focus_area;
}

float Focus::density(CvBox2D box, CvPoint2D32f* pts, int num_pts){
  CvPoint2D32f v_float[4];
  cvBoxPoints(box, v_float);

  CvPoint v[4];
  for (int i = 0; i < 4; ++i){
    v[i] = cvPointFrom32f(v_float[i]);
  }
  
  float count = (float)intersect_count(v, 4, pts, num_pts);

  return count / (float)(box.size.width * box.size.height);
}

float Focus::density(CvConnectedComp comp, CvPoint2D32f* pts, int num_pts){
  CvPoint v[4];
  v[0] = cvPoint(comp.rect.x, comp.rect.y);
  v[1] = cvPoint(comp.rect.x + comp.rect.width, comp.rect.y);
  v[2] = cvPoint(comp.rect.x + comp.rect.width, comp.rect.y - comp.rect.height);
  v[3] = cvPoint(comp.rect.x, comp.rect.y - comp.rect.height);
  
  float count = (float)intersect_count(v, 4, pts, num_pts);
  return count / (float)(comp.rect.width * comp.rect.height);
}

int Focus::intersect_count(CvPoint* verts, int num_verts, 
                           CvPoint2D32f* pts, int num_pts){
  cvZero(poly_img);
  cvZero(point_img);
  cvZero(and_img);
  
  // Draw polygon
  cvFillConvexPoly(poly_img, verts, num_verts, cvScalar(1));
  
  // Draw points
  for (int i = 0; i < num_pts; ++i){
    cvRectangle(point_img, cvPointFrom32f(pts[i]), cvPointFrom32f(pts[i]),
                cvScalar(1), CV_FILLED);
  }
  
  cvAnd(poly_img, point_img, and_img);
  
  int count = cvCountNonZero(and_img);

  return count;
}
