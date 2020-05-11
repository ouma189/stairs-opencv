#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/tracking.hpp>

#include <string.h>
#include <iostream>

using namespace cv;
using namespace std;

bool b = true;
Mat src_gray,src;
int MAX_KERNEL_LENGTH=5;
vector<pair<Point,Point>> vec;

bool compare(pair<Point,Point> i, pair<Point,Point> j) {
    return  (abs(abs(i.second.y)-abs(j.second.y))>25);
}

int main( int argc, char** argv )
{
    src = imread("1.png",IMREAD_COLOR);
    if( src.empty() )
    { help();
      return -1;
    }
    // from color to grayscale
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    
    // gaussian blur
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { GaussianBlur( src_gray, src_gray, Size( i, i ),  0, 0 ); }
    
    // contour filter
    Canny(src_gray, src_gray, 50, 200, 3);
    
    // detect Hough lines
    vector<Vec2f> lines;
    HoughLines(src_gray, lines, 1, CV_PI/180, 150, 0, 0 );
    
    Point pt1, pt2;

    for( size_t i = 0; i < lines.size(); i++ ) {
        // get theta and rho
        float rho = lines[i][0], theta = lines[i][1];
        
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        // sin(theta) * rho - 1000* cos(theta)
        pt1.x = cvRound(x0 + 10000*(-b));
        pt1.y = cvRound(y0 + 10000*(a));
        pt2.x = cvRound(x0 - 10000*(-b));
        pt2.y = cvRound(y0 - 10000*(a));
            
        // if no added lines, add line
        if(vec.empty()){
                vec.push_back(make_pair(pt1,pt2));
            }
        else
            // compare with other lines
            for (auto & v : vec){
                b = b && compare(make_pair(pt1,pt2), v);
            }
        // only add line if it isn't too close to other existing lines
        // rho positive and theta between PI/4 and 3PI/4
        // this is to avoid vertical lines
        if(b){//} && ((rho > 0) &&  (theta < 3*CV_PI/4 && theta > CV_PI/4))){
            vec.push_back(make_pair(pt1,pt2));
            // draw line
            line(src, pt1, pt2, Scalar(0,0,255), 4, LINE_AA);
        }
    }
    // show number of lines
    putText(src, to_string(vec.size()-1) , cvPoint(50,50),
                FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(200,200,250), 2, LINE_AA);
           
    imshow("detected lines", src_gray);
    waitKey();
    return 0;
    
}



