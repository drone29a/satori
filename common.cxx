#include "common.h"
#include "img_template.tpl"	// provides efficient access to pixels

void draw_box(const CvBox2D* box, IplImage* img, const CvScalar& color){
  CvPoint2D32f v[4];
  cvBoxPoints(*box, v);
  
  cvRectangle(img, cvPointFrom32f(v[0]), cvPointFrom32f(v[2]), 
              color, CV_FILLED);
}

void draw_comp(const CvConnectedComp* comp, IplImage* img, const CvScalar& color){
  cvRectangle(img, 
              cvPoint(comp->rect.x, comp->rect.y), 
              cvPoint(comp->rect.x + comp->rect.width, comp->rect.y + comp->rect.height),
              color, CV_FILLED);
}

void draw_points(CvPoint2D32f* pts, int num_pts, IplImage* img, const CvScalar& color){
  for (int i = 0; i < num_pts; ++i){
     cvRectangle(img, cvPointFrom32f(pts[i]), cvPointFrom32f(pts[i]),
                 cvScalar(255), CV_FILLED);
  }
}

float intersect_amount(IplImage* x, IplImage* y, IplImage* dst){
  // the percentage of intersection is wrt the first image
  cvAnd(x, y, dst);

  CvRect db = cvBoundingRect(dst);
  CvRect xb = cvBoundingRect(x);
  CvRect yb = cvBoundingRect(y);
  
  return (float)(db.width * db.height) / (float)(xb.width * xb.height);
}
