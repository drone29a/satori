/*
 * img_template.tpl - Template for direct access to image elements
 * (c) 2008 Michael Sullivan
 *
 * Version 1.0.0
 * Last Revised: 04/30/08
 *
 * Version History:
 *   1.0.0 - Initial implementation (04/30/08)
 *
 * Code taken from:
 * (1) Gady Agam's Introduction to programming with OpenCV (available at: http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/index.html)
 *
 */
template<class T> class Image
{
  private:
  IplImage* imgp;
  public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  void operator=(IplImage* img) {imgp=img;}
  inline T* operator[](const int rowIndx) {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
  unsigned char b,g,r;
} RgbPixel;

typedef struct{
  float b,g,r;
} RgbPixelFloat;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;


