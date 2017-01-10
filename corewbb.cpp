#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/legacy/legacy.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <windows.h>
#include <assert.h>

#include "cubedetect.hpp"
#include "surfdetect.hpp"
#include "colordetect.hpp"



#pragma comment(lib, "winmm.lib")

using namespace cv;
using namespace std;


int flag=0;
//char* filename;
char key;
// int objectcount=0;

int main()
{

     VideoCapture capture(1); 
	 assert(capture.isOpened());
	  Mat src;
	 while (1) {
        
		  capture >> src;
          key= cvWaitKey(3);
	//	cout<<key<<endl;
			imshow("Camera",src);
			cvWaitKey(1);
		 if(key==27)
	 		 break;

	// { 
		 
	//	 sprintf(filename,"Picture %d.bmp",++objectcount);
	//	 imwrite(filename,src);
	//	 imshow("Camera",src);
		
	//	 cvWaitKey(200);
	// }
	//  cin>>flag;
	//	flag=int (key);
		switch (key){
		case 'a': 
		{ //只有木块，只需识别颜色
			    COLOR=colordetect(src);
				switch (COLOR)
		    case 0: cout<<"no color detect"<<endl; break;
			case 1: PlaySound("yellowcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 2: PlaySound("redcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 3: PlaySound("greencube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 4: PlaySound("bluecube.wav",NULL,SND_FILENAME | SND_SYNC );break;

		}break;
     
		case 'b': 
		{//易拉罐和木块的区分
                  cubefound = cubedetect(src);
				  if (cubefound )
				  {
					  COLOR=colordetect(src);
				      switch (COLOR){
			case 0: cout<<"no color detect"<<endl; break;
			case 1: PlaySound("yellowcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 2: PlaySound("redcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 3: PlaySound("greencube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 4: PlaySound("bluecube.wav",NULL,SND_FILENAME | SND_SYNC );break;
					  }	    
				   }
				  else{
					   canfound=surfdetect(src);
					   switch (canfound){
				   case 10: cout<<"no can detect"<<endl;;break;
				   case 0:PlaySound("WLJ.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 1:PlaySound("JDB.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 2:PlaySound("XM.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 3:PlaySound("MND.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 4:PlaySound("FD.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 5:PlaySound("FDKXC.wav",NULL,SND_FILENAME | SND_SYNC );break;
					   }

				  }

		}break;
             
            case 'c':   //木块和书本的区分 
			{
                 cubefound= cubedetect(src);
                  if (cubefound )
				  {
					  COLOR=colordetect(src);
				      switch( COLOR){//播报什么颜色的木块
			case 0: cout<<"no color detect"<<endl; break;
			case 1: PlaySound("yellowcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 2: PlaySound("redcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 3: PlaySound("greencube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 4: PlaySound("bluecube.wav",NULL,SND_FILENAME | SND_SYNC );break;
				}
				   }
				  else{
					   COLOR=colordetect(src);
				      switch( COLOR){//播报什么颜色的书本
		    case 0: cout<<"no color detect"<<endl; break;
			case 2: PlaySound("redbook.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 3: PlaySound("greenbook.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 4: PlaySound("bluebook.wav",NULL,SND_FILENAME | SND_SYNC );break;
					  }
				  }
		  }break;
         case 'd':    
	     { //易拉罐和书的区分
                  canfound=surfdetect(src);
				 if(!canfound){
				 switch (canfound){
				   case 10: cout<<"no can detect"<<endl;;break;
				   case 0:PlaySound("WLJ.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 1:PlaySound("JDB.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 2:PlaySound("XM.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 3:PlaySound("MND.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 4:PlaySound("FD.wav",NULL,SND_FILENAME | SND_SYNC );break;
				   case 5:PlaySound("FDKXC.wav",NULL,SND_FILENAME | SND_SYNC );break;
				    }
				 }
				 else{
					  COLOR=colordetect(src);
				      switch( COLOR){//播报什么颜色的木块
			case 0: cout<<"no color detect"<<endl; break;
			case 1: PlaySound("yellowcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 2: PlaySound("redcube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 3: PlaySound("greencube.wav",NULL,SND_FILENAME | SND_SYNC );break;
			case 4: PlaySound("bluecube.wav",NULL,SND_FILENAME | SND_SYNC );break;
					  } 
				 }
				
                 
		 }break;
		}
	//_CrtDumpMemoryLeaks();

  }
	 return 0;
}