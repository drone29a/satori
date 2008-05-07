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
    float area = 0.f, cam_amt = 0.f, seg_amt = 0.f;
    intersect_amount(point_img, poly_img, and_img,
                     area, cam_amt, seg_amt);
    CvRect camshift_rect = cvBoundingRect(poly_img);
    CvRect seg_rect = cvBoundingRect(point_img);
    float cam_seg_size_ratio = camshift_rect.width*camshift_rect.height / seg_rect.width*seg_rect.height;
    float cam_frame_size_ratio = (float)(camshift_rect.width*camshift_rect.height) / (float)(FRAME_SIZE.width*FRAME_SIZE.height);
    
    // Calculate feature density for both CAMSHIFTed box and motion segment
    float cam_density = density(track_box, feature_points, num_points);
    float seg_density = density(motion_seg, feature_points, num_points);
    float seg_cam_density_ratio = seg_density / cam_density;
  
    // Decide whether to change focus
    if (//(seg_cam_density_ratio > 1.08f || seg_cam_point_count_ratio > 0.7) && 
        cam_frame_size_ratio > 0.4f && 
        (seg_amt < 0.65f || cam_amt < 0.65f)) {
      changed = true;
    }
        
    cout << "density ";
    
    if (seg_cam_density_ratio > 1.08f){
      cout << "CHANGE ";
    }
    else{
      cout << "STAY ";
    }

    cout << " | ";
    cout << "size ";

    if (cam_frame_size_ratio > 0.4f){
      cout << "CHANGE";
    }
    else{
      cout << "STAY";
    }

    cout << " | ";
    cout << "segment intersect ";

    if (seg_amt < 0.65f){
      cout << "CHANGE";
    }
    else{
      cout << "STAY";
    }  

    cout << " | ";
    cout << "camshift intersect ";

    if (cam_amt < 0.65f){
      cout << "CHANGE" << endl;
    }
    else{
      cout << "STAY" << endl;
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
  draw_points(pts, num_pts, point_img, cvScalar(255));
  
  cvAnd(poly_img, point_img, and_img);

  int count = cvCountNonZero(and_img);

  return count;
}
