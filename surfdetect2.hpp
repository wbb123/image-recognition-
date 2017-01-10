#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>

using namespace cv;
using namespace std;


int canfound = 9;
int surfdetect(Mat scene)
{
	//设置不同饮料的标志位，用静态变量
	static int f[6]={0,0,0,0,0,0};
	//这里数组要有个指针，并不懂
	 string pic[6][3] = {{ "WLJ.bmp","WLJ2.bmp", "WLJ3.bmp"},
{"JDB.bmp", "JDB2.bmp","JDB3.bmp"},
{"XM.bmp","XM2.bmp","XM3.bmp"},
{"MND.bmp","MND2.bmp","MND3.bmp"},
{"FD.bmp","FD2.bmp","FD3.bmp"},
{"FDKXC.bmp","FDKXC2.bmp","FDKXC3.bmp" } };
	int j=0,k=0,m=0;

	//高斯滤波
	Mat out1,out2;
	GaussianBlur(scene,out2,Size(7,7),0,0);
	//判断标志位，如果为1，不再匹配该模板
	
	for(j=0;j<6;j++)
	{			
		if(f[j]==1) continue;
		else{

			for(k=0;k<3;k++){
				
			out1=imread(pic[j][k],1);
			
			//imshow("A",out1);
			//【2】使用SURF算子检测关键点  
			int minHessian = 1000;//SURF算法中的hessian阈值
			SurfFeatureDetector detector( minHessian );//定义一个SurfFeatureDetector（SURF） 特征检测类对象    
			std::vector<KeyPoint> keyPoint1, keyPoints2;//vector模板类，存放任意类型的动态数组  
			//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中  
			detector.detect( out1, keyPoint1 );
			detector.detect(out2, keyPoints2);  
			//【4】计算描述符（特征向量）  
			SurfDescriptorExtractor extractor;  
			Mat descriptors1, descriptors2;  
			extractor.compute( out1, keyPoint1, descriptors1 );  
			extractor.compute( out2, keyPoints2, descriptors2 );  
			//【5】FLANN匹配
			FlannBasedMatcher matcher;
			vector<DMatch> matches;
			matcher.match(descriptors1, descriptors2, matches);
			//计算最大和最小距离
			double max_dist = 0;
			double min_dist = 100;
			for (int i = 0; i < descriptors1.rows; i++) {
				double dist = matches[i].distance;
				if (dist < min_dist)
					min_dist = dist;
				if (dist > max_dist)
					max_dist = dist;
			}
//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
//-- PS.- radiusMatch can also be used here.
			std::vector< DMatch > good_matches;
			for( int i = 0; i < descriptors1.rows; i++ )
			{ if( matches[i].distance < 2*min_dist )
			{ good_matches.push_back( matches[i]); }
			}
			//-- Draw only "good" matches
			Mat img_matches;
			drawMatches(out1, keyPoint1, out2, keyPoints2,
				good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
			
			//-- Localize the object from img_1 in img_2
			std::vector<Point2f> obj;
			std::vector<Point2f> scene;
			for( int i = 0; i < good_matches.size(); i++ )
			{
				//-- Get the keypoints from the good matches
				obj.push_back( keyPoint1[ good_matches[i].queryIdx ].pt );
				scene.push_back( keyPoints2[ good_matches[i].trainIdx ].pt );
			}
			Mat H = findHomography( obj, scene, CV_RANSAC );
			//-- Get the corners from the image_1 ( the object to be "detected" )
			Point2f obj_corners[4] = { cvPoint(0,0), cvPoint( out1.cols, 0 ), 
				cvPoint( out1.cols,out1.rows ),	cvPoint( 0, out1.rows ) };
			Point scene_corners[4];
			//-- Map these corners in the scene ( image_2)
			for( int i = 0; i < 4; i++ )
			{
				double x = obj_corners[i].x;
				double y = obj_corners[i].y;
				double Z = 1./( H.at<double>(2,0)*x + H.at<double>(2,1)*y + H.at<double>(2,2) );
				double X = ( H.at<double>(0,0)*x + H.at<double>(0,1)*y + H.at<double>(0,2) )*Z;
				double Y = ( H.at<double>(1,0)*x + H.at<double>(1,1)*y + H.at<double>(1,2) )*Z;
				scene_corners[i] = cvPoint( cvRound(X) + out1.cols, cvRound(Y) );
			}
			//计算匹配到的图形的面积大小，并以此来判断
			double area=(scene_corners[1].x-scene_corners[0].x)*(scene_corners[3].y-scene_corners[0].y);
			double area0=(obj_corners[1].x-obj_corners[0].x)*(obj_corners[3].y-obj_corners[0].y);
			//cout<<area<<endl;
			if(area>(area0/1.5)&&area<(1.5*area0))
			{
				//-- Draw lines between the corners (the mapped object in the scene - image_2 
				line( img_matches, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 2 );
				line( img_matches, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 2 );
				line( img_matches, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 2 );
				line( img_matches, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 2 );
				//-- Show detected matches
				//imshow( "Good Matches & Object detection", img_matches );
				//waitKey(1);

				//将正反面两个模板的标志位置一
//				m=int(j/2);
//				f[2*m]=1;
//				f[2*m+1]=1;
//				return m;	
				f[j]=1;
				return j;
			}
			}
		}
	}
	return j;
	}