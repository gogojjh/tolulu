#include "base.h"

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 50;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
int c = 0;

void counter_contours(int i, vector<Vec4i> hierarchy)
{
    c++;
    if (hierarchy[i][2]!=-1)
    {
        counter_contours(hierarchy[i][2], hierarchy);
    }
    if (hierarchy[i][0]!=-1)
    {
        counter_contours(hierarchy[i][0], hierarchy);
    }
}

int main()
{
  VideoCapture capture("/home/gogojjh/QT/mapRecognition/led3/test1.avi");
  Mat frame;

  while (1)
  {
      capture >> frame;
      //frame = imread( "/home/gogojjh/QT/mapRecognition/led3/82.jpg", 1 );
      resize(frame,frame,Size(frame.cols/2, frame.rows/2));
      Mat result;
      frame.copyTo(result);
      medianBlur(frame,frame, 9);

      //v
      Mat imgV = colorConversion(frame, HSV_V);
      threshold(imgV, imgV, 160 , 255, THRESH_BINARY );
      Mat element = getStructuringElement(MORPH_RECT,Size(5,5));
      morphologyEx(imgV, imgV,MORPH_DILATE,element);
//      imshow("V", imgV);

      //canny
      Mat detected_edges;
      Canny( imgV, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
      detected_edges = ~detected_edges;
      element = getStructuringElement(MORPH_CROSS,Size(5,5));
      morphologyEx(detected_edges, detected_edges,MORPH_ERODE,element);
      imshow( "canny", detected_edges );

      //contours
      vector<vector<Point> > contours;
      vector<Vec4i> hierarchy;
      findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
      Mat imgContours(frame.rows, frame.cols, CV_8UC1, Scalar(0));
     drawContours(imgContours, contours, -1, Scalar(255), 1);
     imshow("imgContour", imgContours);

      int maxContourSize = 0;
      int d = 0;
      int ans = 0; //0 hippo 1 octopus
      for (int i=0; i<contours.size(); i++)
      {
          float contourSize = contourArea(contours[i]);
          if (contourSize < 5000) continue;
          if (contourSize >maxContourSize)
          {
              maxContourSize = contourSize;
              d = i;
          }
          c = 0;
          if (hierarchy[i][2]!=-1) counter_contours(hierarchy[i][2], hierarchy);
          if (c <= 4) continue;
          ans = 1;
      }
      imshow("src", frame);
      if (ans==0) cout << "hippo" << endl;
                 else cout << "octopus" << endl;
      waitKey(0);
  }
  return 0;
}
