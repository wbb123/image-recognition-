#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include"opencv2/legacy/legacy.hpp"
#include <iostream>
#include <stdio.h>
#include "opencv2/calib3d/calib3d.hpp"

#ifndef _CUBEDETECT_HPP_
#define _CUBEDETECT_HPP_

using namespace cv;
using namespace std;

bool cubefound = false;

Mat& BinaryimageUseOSTU(Mat& src_gray, Mat&src_binary);
static bool isRectangle(vector<Point> polygon);
static bool isSquare(vector<Point> polygon);
static double polyArea(vector<Point> polygon);
static double angle(Point p1, Point p2, Point p3);
static void drawPolygon(Mat canvas, vector<Point> polygon);
Mat& BinaryimageUseOSTU(Mat& src_gray, Mat&src_binary)
{
	// accept only char type matrices  
	CV_Assert(src_gray.depth() != sizeof(uchar)); 

	int height=src_gray.rows;
	int width =src_gray.cols;
	long N=height*width;
	
	int h[256]={0};
	double p[256]={0},u[256]={0},w[256]={0};

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			h[(int)src_gray.at<uchar>(i,j)]++;//统计每个灰度级的数目
		}
	}

	for(int i = 0; i < 256; i++)
		p[i] = h[i] / double(N);

	int T = 0;
	double T1,S1;
	double S1_max = -10000;
	for(int k = 0; k < 256; k++)
	{
		T1 = 0;
		for(int i = 0; i <= k; i++)
		{
			u[k] += i*p[i];
			w[k] += p[i];
		}
		for(int i = 0; i < 256; i++)
			T1 += i*p[i];

		S1 = (T1*w[k] - u[k])*(T1*w[k] - u[k]) / (w[k]*(1-w[k]));

		if(S1 > S1_max)
		{
			S1_max = S1;
			T = k;
		}
	}

	for(int i = 0; i < height; i++)
	{ 
		for(int j = 0; j < width; j++)
		{
			if(src_binary.at<uchar>(i,j) > T)
			{
				src_binary.at<uchar>(i,j)  = 255;
			}
			else
			{
				src_binary.at<uchar>(i,j) = 0;
			}
		}
	}
	return src_binary;
} 
 static bool isSquare(vector<Point> polygon)
	{
		if (polygon.size() != 4)
			return false;

		bool ok = true;
		vector<double> len;
		for (int i  = 0; i < 4; i++) {
			//If the cos of the angle is less than the Threshold, 
			//we can say that it is a rignt angle.
			if (fabs(angle(polygon[(i+3)%4], polygon[i], polygon[(i+1)%4])) > 0.2) {
				ok = false;
				break;
			}

			Point p1 = polygon[i];
			Point p2 = polygon[(i+1)%4];
			double dx = p1.x - p2.x;
			double dy = p1.y - p2.y;
			len.push_back(sqrt(dx * dx + dy * dy));
		}

		if (ok) {
			vector<double> mean, stddev;
			cv::meanStdDev(len, mean, stddev);
		
			if (stddev[0] > 30)     //MAX_SQUARE_STDDEV=30
				ok = false;
		}

		return ok;
	}
static double angle(Point p1, Point p2, Point p3)
	{
		Vec2i v1 = p1 - p2, v2 = p3 - p2;
		return (double)v1.dot(v2) / (norm(v1) * norm(v2));
	}
static void drawPolygon(Mat canvas, vector<Point> polygon)
	{
		int n = polygon.size();

		if (n == 0)
			return;

		Point pre = polygon[n-1];
		for (int i = 0; i < n; i++) {
			Point cur = polygon[i];
			line(canvas, pre, cur, Scalar(0, 0, 0));
			cv::namedWindow("approxs");
			imshow("approxs",canvas);
			pre = cur;
		}
	}
static double polyArea(vector<Point> polygon)
	{
		if (polygon.size() == 0)
			return 0.0;

		Point pre = polygon[polygon.size() - 1];
		double area = 0.0;
		for (unsigned int i = 0; i < polygon.size(); i++) {
			Point cur = polygon[i];
			area += cur.x * pre.y - cur.y * pre.x;
			pre = cur;
		}

		area = abs(area) / 2;

		return area;
	}
static bool isRectangle(vector<Point> polygon)
	{
		if (polygon.size() != 4)
			return false;

		bool ok = true;
		for (int i  = 0; i < 4; i++) {
			//If the cos of the angle is less than the Threshold, 
			//we can say that it is a rignt angle.
			if (fabs(angle(polygon[(i+3)%4], polygon[i], polygon[(i+1)%4])) > 0.2) {//_angleThreshold 
				ok = false;
				break;
			}
		}

		return ok;
	}

bool cubedetect(Mat&canvas)
{
	   cubefound= false;
		Mat src,src_gray;
		int minArea=10000;
        int maxArea=20000;
#pragma region OSTU

	   src=canvas;
	   cvtColor(src,src_gray,CV_RGB2GRAY);  //转换为灰度图像
	 //OSTU提取对象
		Mat src_binary=src_gray;//赋值为灰度图像
		BinaryimageUseOSTU(src_gray,src_binary);  
		//	imshow("OSTU_IMAGE1",src_binary);
		
		src_binary=src_gray;//赋值为灰度图像
		threshold(src_gray,src_binary,0,255,THRESH_OTSU);//opencv2.4.5自带OSTU函数二值化
		
	//	imshow("OSTU_IMAGE2",src_binary);
	//	waitKey(3000);//显示3秒
	//	imwrite("binaryGroup.bmp",src_binary);
	    Mat image=src_binary;
		// Get the contours of the connected components
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(image, 
		contours, // a vector of contours 
		 CV_RETR_LIST, // retrieve the external contours
		CV_CHAIN_APPROX_SIMPLE); // retrieve all pixels of each contours
  cv::Mat result(image.size(),CV_8U,cv::Scalar(255));
	// Print contours' length
	if( contours.size()>0){
		std::cout << "Contours: " << contours.size() << std::endl;
	    std::vector<std::vector<cv::Point>>::const_iterator itContours= contours.begin();
	 //   for ( ; itContours!=contours.end(); ++itContours) {

	//	std::cout << "Size: " << itContours->size() << std::endl;

 //	}
	// draw black contours on white image
	
	cv::drawContours(result,contours,
		-1, // draw all contours
		cv::Scalar(0), // in black
		2); // with a thickness of 2

//	cv::namedWindow("Contours");
//	cv::imshow("Contours",result);
//	imwrite("contour.bmp",result);
}
	else
		cout<<"no contour found"<<endl;
	
	    vector<vector<Point> >approxs;
		vector<Point> approx;
		vector<vector<Point> > squares;

		for (unsigned int i = 0; i < contours.size(); i++) {
			//Approximation with using Douglas-Peucker algorithm
		approxPolyDP(contours[i], approx, 10, true);   //approxEps=10;
		  approxs.push_back(approx);
			
		}

	if (approxs.size()>0){
		drawPolygon(result, approx);
		cout<<approxs.size()<<"approxs"<<endl;}

		for (unsigned int i = 0; i < approxs.size(); i++) {

			if (isSquare(approxs[i])) {     //angleThreshold =0.2
				double approx_area = polyArea(approxs[i]);
				cout<< "area=" << approx_area<<endl;
				if (approx_area > minArea && approx_area < maxArea)
					squares.push_back(approxs[i]);
			}
		}
		
		 cv::Mat result1(image.size(),CV_8U,cv::Scalar(255));
		if(squares.size()>0){
		  cout << squares.size() << " square(s) found."<<endl;
		  cubefound= true;
		for (unsigned int i = 0; i < squares.size(); i++) {
				Scalar color(0, 255, 255);
				for (int j = 0; j < 4; j++) {
					line(result1, squares[i][j], squares[i][(j+1)%4], color);
					circle(result1, squares[i][j], 10, color, 3);
					
				}
			}
		   cv::namedWindow("last");
		   imshow("last",result1);
		}
	    	
	return cubefound;

}
#endif