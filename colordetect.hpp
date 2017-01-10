#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>

#ifndef _COLORDETECT_HPP_
#define _COLORDETECT_HPP_


using namespace cv;
using namespace std;
int 	  x=0,y=0;
static int H_MIN[4]={16,130,40,100};     //  yellow red green blue
static int S_MIN[4]={100,60,60,60}; 
static int V_MIN[4]={0,0,0,20}; 
static int H_MAX[4]={30,180,80,120};
static int S_MAX[4]={250,256,256,256};
static int V_MAX[4]={255,256,256,256};

int COLOR;
bool objectFound= false;
const int MAX_NUM_OBJECTS=50;
const int MIN_OBJECT_AREA = 20*20;

void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed);
void morphOps(Mat &thresh);
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){

	    const int FRAME_HEIGHT=threshold.size().height;
		const int FRAME_WIDTH=threshold.size().width;

	    const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH;
        Mat temp;
        threshold.copyTo(temp);
        //these two vectors needed for output of findContours
        vector< vector<Point> > contours;
        vector<Vec4i> hierarchy;
        //find contours of filtered image using openCV findContours function
        findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	    //use moments method to find our filtered object
       double refArea = 0;
    
        if (hierarchy.size() > 0) {
                int numObjects = hierarchy.size();
                //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
                if(numObjects<MAX_NUM_OBJECTS){
                        for (int index = 0; index >= 0; index = hierarchy[index][0]) {

                                Moments moment = moments((cv::Mat)contours[index]);
                                double area = moment.m00;

                                //if the area is less than 20 px by 20px then it is probably just noise
                                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                                //we only want the object with the largest area so we safe a reference area each
                           //iteration and compare it to the area in the next iteration.
                                if(area && area<MAX_OBJECT_AREA && area>refArea){
                                        x = moment.m10/area;
                                        y = moment.m01/area;
                                        objectFound = true;
                                        refArea = area;
										//cout<<area<<endl;
                                }else objectFound = false;
                        }				
        }
     }

}

void morphOps(Mat &thresh){

        //create structuring element that will be used to "dilate" and "erode" image.
        //the element chosen here is a 3px by 3px rectangle

        Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
        //dilate with larger element so make sure object is nicely visible
        Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

        erode(thresh,thresh,erodeElement);
        erode(thresh,thresh,erodeElement);


        dilate(thresh,thresh,dilateElement);
        dilate(thresh,thresh,dilateElement);
}




int colordetect(Mat &canvas){
    COLOR=0;
	objectFound= false;
	
      Mat threshold,HSV;
	 cvtColor(canvas,HSV,COLOR_BGR2HSV);
int 	  x=0,y=0;
	 //判断是否为黄色
	 inRange(HSV,Scalar(H_MIN[0],S_MIN[0],V_MIN[0]),Scalar(H_MAX[0],S_MAX[0],V_MAX[0]),threshold);
     morphOps(threshold);
	 trackFilteredObject(x,y,threshold,canvas);
	 if(objectFound == true){
		cout<<"yellowFound"<<endl;
		COLOR=1; return COLOR;
	
	 }
	  
      //判断是否为红色
	 inRange(HSV,Scalar(H_MIN[1],S_MIN[1],V_MIN[1]),Scalar(H_MAX[1],S_MAX[1],V_MAX[1]),threshold);
     morphOps(threshold);
	    x=0,y=0;
	 trackFilteredObject(x,y,threshold,canvas);
	 if(objectFound == true){
		cout<<"redFound"<<endl;
		COLOR=2; return COLOR;
		}

		 //判断是否为绿色
	 inRange(HSV,Scalar(H_MIN[2],S_MIN[2],V_MIN[2]),Scalar(H_MAX[2],S_MAX[2],V_MAX[2]),threshold);
    morphOps(threshold);
	   x=0,y=0;
	 trackFilteredObject(x,y,threshold,canvas);
	 if(objectFound == true){
		cout<<"greenFound"<<endl;
		COLOR=3; return COLOR;
		}

	   //判断是否为蓝色
	 inRange(HSV,Scalar(H_MIN[3],S_MIN[3],V_MIN[3]),Scalar(H_MAX[3],S_MAX[3],V_MAX[3]),threshold);
    morphOps(threshold); 
	 x=0,y=0;
	 
	 trackFilteredObject(x,y,threshold,canvas);
	 if(objectFound == true){
		cout<<"blueFound"<<endl;
		COLOR=4; return COLOR;
		}
	return COLOR;
}

#endif