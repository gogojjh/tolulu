#include "base.h"

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 30;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;

int angle( Point p1, Point p0, Point p2)
{
    float dx1 = p1.x - p0.x;
    float dy1 = p1.y - p0.y;
    float dx2 = p2.x - p0.x;
    float dy2 = p2.y - p0.y;
    float angle_line = (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);//余弦值
    float s = acos(angle_line)*180/CV_PI;
    //cout << s << endl;
    if ((s>80) && (s <100)) return 1;
    else return 0;
}

int main()
{
  VideoCapture capture("/home/gogojjh/QT/mapRecognition/data819/0819.avi");
  Mat frame;

  while (1)
  {
      capture >> frame;
      //frame = imread( "/home/gogojjh/QT/mapRecognition/data819/338.jpg", 1 );
      resize(frame,frame,Size(frame.cols/2, frame.rows/2));
      Mat result;
      frame.copyTo(result);
      medianBlur(frame,frame,5);

      dst.create(frame.rows, frame.cols, CV_8UC1);
      dst = Scalar::all(0);
//      namedWindow("1contour", 2);
//      namedWindow("canny ", 2);

      cvtColor( frame, src_gray, CV_BGR2GRAY );
      //imshow("gray", src_gray);
      blur( src_gray, detected_edges, Size(3,3));
      Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
      detected_edges = ~detected_edges;

      /*
      line(detected_edges, Point(0,0), Point(detected_edges.cols-1,0), Scalar(0), 1);
      line(detected_edges, Point(0,0), Point(0,detected_edges.rows-1), Scalar(0), 1);
      line(detected_edges, Point(detected_edges.cols-1,0), Point(detected_edges.cols-1,detected_edges.rows-1), Scalar(0), 1);
      line(detected_edges, Point(0,detected_edges.rows-1), Point(detected_edges.cols-1,detected_edges.rows-1), Scalar(0), 1);
      Mat element = getStructuringElement(MORPH_RECT,Size(2,2));
      morphologyEx(detected_edges, detected_edges,MORPH_ERODE,element);
      */

      Mat element = getStructuringElement(MORPH_CROSS,Size(3,3));
      morphologyEx(detected_edges, detected_edges,MORPH_ERODE,element);
      imshow( "canny", detected_edges );

      vector<vector<Point> > contours;
      vector<Vec4i> hierarchy;
      findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
      Mat imgContours(frame.rows, frame.cols, CV_8UC1, Scalar(0));
      //drawContours(imgContours, contours, -1, Scalar(255), 1);
//      imshow("contour", imgContours);

      int maxContourSize = 0;
      vector<Point> approxCurve;
      RotatedRect box, box1, box2;
      int boxarea;
      int s1, s2, s3, s4;
      Point2f v[4];
      for (int i=0; i<contours.size(); i++)
      {
          drawContours(imgContours, contours, i, Scalar(255), 1);
          float contourSize = contourArea(contours[i]);
          float contourLength = arcLength(contours[i], 0);
          //cout << contourSize << endl;
          if (contourSize < frame.rows * frame.cols * 0.04) continue;
          if (contourSize > frame.rows * frame.cols * 0.8) continue;

          approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);
          //cout<<"pppppp:"<<approxCurve.size()<<endl;
          if (! isContourConvex(Mat(approxCurve))) continue;

//          if (pow(contourLength/4, 2.0) - contourSize > contourSize*0.5) continue;

          drawContours(imgContours, contours, i, Scalar(255), 1);

//          cout << contourSize << endl;
//          cout << arcLength(contours[i], 0) << endl;

          box = minAreaRect(contours[i]);

//          if (abs(contourSize-box.size.width*box.size.height) )

          Point2f vertex[4];
          box.points(vertex);
          s1 = angle(vertex[0], vertex[1], vertex[2]);
          s2 = angle(vertex[1], vertex[2], vertex[3]);
          s3 = angle(vertex[2], vertex[3], vertex[0]);
          s4 = angle(vertex[3], vertex[0], vertex[1]);
          if ((!s1) || (!s2) || (!s3) || (!s4)) continue;
          if (contourSize > maxContourSize)
          {
              maxContourSize = contourSize;
              v[0] = vertex[0]; v[1] = vertex[1]; v[2] = vertex[2]; v[3] = vertex[3];
          }
      }
      //imshow("contour", imgContours);

      Point goal(0,0);
      if (maxContourSize != 0)
      {
          for(int i=0;i<4;i++)
          {
              line(result, v[i], v[(i+1)%4], Scalar(0,0,255), 3);
          }
          goal.x = (v[0].x + v[1].x + v[2].x + v[3].x) / 4;
          goal.y = (v[0].y + v[1].y + v[2].y + v[3].y) / 4;
          circle(result, goal, 3,Scalar(0,0,255));
      }
      imshow("result", result);
      //imshow( "src", frame);

      //createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
      //CannyThreshold(0, 0);
      waitKey(0);
  }
  return 0;
}
