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

void intersect_amount(IplImage* x, IplImage* y, IplImage* dst,
                      float& area, float& x_amt, float& y_amt){
  // Finds the area, percentage of x, and percentage of y involved
  // intersection.
  IplImage* tmp = cvCreateImage(cvGetSize(dst), 8, 1);

  cvZero(dst);
  cvAnd(x, y, dst);

  CvRect db = cvBoundingRect(dst);
  CvRect xb = cvBoundingRect(x);
  CvRect yb = cvBoundingRect(y);

  cvZero(tmp);
  cvAnd(x, dst, tmp);
  CvRect xib = cvBoundingRect(tmp);

  cvZero(tmp);
  cvAnd(y, dst, tmp);
  CvRect yib = cvBoundingRect(tmp);
  
  cvRectangle(dst, 
              cvPoint(db.x, db.y), 
              cvPoint(db.x+db.width, db.y+db.height), 
              cvScalar(255), CV_FILLED);

  cvReleaseImage(&tmp);
  
  area = (float)(db.width * db.height);
  x_amt = ((float)xib.width*xib.height) / ((float)xb.width*xb.height);
  y_amt = ((float)yib.width*yib.height) / ((float)yb.width*yb.height);
}

void rect_to_points(const CvRect& rect, CvPoint points[]){
  points[0] = cvPoint(rect.x, rect.y);
  points[1] = cvPoint(rect.x+rect.width, rect.y);
  points[2] = cvPoint(rect.x+rect.width, rect.y+rect.height);
  points[3] = cvPoint(rect.x, rect.y+rect.height);
}
