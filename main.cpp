//Computer Vision 159731 2018 Mini project
//Designed by: Jerry Wang(Student ID: 02304430) and Rebecca Liu(Student ID:17238744)
//Date: 18/Apr/2018 - 19/June/2018
//Program can be compiled as below:
//g++ -std=c++11 -o main main.cpp `pkg-config --cflags --libs opencv`
//or use make with makefile shown below
//   CC = g++
//   CFLAGS = -std=c++11
//   PROG = main
//   SRCS = $(PROG).cpp
//   PROGS = ./$(PROG)
//   OBJS=$(PROG).o
//   OPENCV = `pkg-config opencv --cflags --libs`
//   LIBS = $(OPENCV)
//   $(PROG):$(SRCS)
//      $(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
//Usage:
// miniproj -HAAR/-LBP [directory or file name]

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <stdio.h>
#include <chrono>

using namespace std;
using namespace cv;
using namespace chrono;

/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */
String haar_car_cascade_name = "haarcascade_car.xml";
String lbp_car_cascade_name = "lbpcascade_car.xml";

String haar_face_cascade_name = "haarcascade_frontalface_alt.xml";
String lbp_face_cascade_name = "lbpcascade_frontalface_alt.xml";

CascadeClassifier car_cascade, face_cascade;

string window_name = "Passengers in Car Detector ";

string subtitleHaar="- Haar";
string subtitleLBP="- LBP";
float fps = 0.0;
int testingImgDir=0;
int HAAR;

void detectAndDisplay(Mat mat);
int TestImgDir(const char *dirname);

/* return 1 if it is really a directory name, otherwise return 0 */
int TestImgDir(const char *dirname)
{   
    DIR *dir = opendir(dirname);
    struct dirent *ent;
    string image_filename;

   if (dir == NULL)
       return 0;
   else
   {
        testingImgDir=1;
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strstr(ent->d_name,".jpg")!=NULL) {
                image_filename=ent->d_name;
                printf ("filename: %s\n", image_filename.c_str());
                char imagefile[80];
                sprintf(imagefile,"%s/%s",dirname,image_filename.c_str());
                Mat img = imread(imagefile, 1);
                if( img.empty()) {
                    cout << "Couldn't load " <<imagefile << endl;
                }
                else {
                    detectAndDisplay(img);
                }
            }
        }
	
	free(ent);
	closedir(dir);
        return 1;
    }
}


int main( int argc, const char * argv[] )
{   VideoCapture capture;
    Mat frame;

	if (argc == 1)
	{
		printf("usage: %s -HAAR/-LBP [directory or file name]\n", argv[0]);
		return 0;
	}

	if (!strcmp(argv[1], "-HAAR"))
		HAAR = 1;
	else if (!strcmp(argv[1], "-LBP"))
		HAAR = 0;
	else
	{
		printf("usage: %s -HAAR/-LBP [directory or file name]\n", argv[0]);
		return 0;
	}	

	window_name += HAAR ? subtitleHaar : subtitleLBP;
	if (!car_cascade.load(HAAR ? haar_car_cascade_name : lbp_car_cascade_name))
	{
		printf("--Error loading cascade classfier file %s\n", (HAAR ? haar_car_cascade_name : lbp_car_cascade_name).c_str());
		return -1;
	}
	
	if (!face_cascade.load(HAAR ? haar_face_cascade_name : lbp_face_cascade_name))
	{
		printf("--Error loading cascade classfier file %s\n", (HAAR ? haar_face_cascade_name : lbp_face_cascade_name).c_str());
		return -1;
	}

	if (argc == 2) // webcam
	{  
        capture.open(0);
        Mat flipimg;
        if( capture.isOpened())
        {  capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
           capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
           printf("video frame width x height: %f x %f\n", capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT));
           fps=0.0;
           do
           {   system_clock::time_point start = system_clock::now();
               capture.read(flipimg);
               //Apply the classifier to the frame
               if( !flipimg.empty() )
               {   //flip image
                   flip(flipimg,frame,1);
                   //add fps to frame
                   char text[80];
                   sprintf(text,  "%2.1f", fps);
                   putText(frame, text, cvPoint(10,80), FONT_HERSHEY_PLAIN, 2, cvScalar(0,0,255),2,8);
                   detectAndDisplay(frame);
                   system_clock::time_point end = system_clock::now();
                   float seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                   fps = 1000000 / seconds;
               }
               else
               {   printf("Warning -- No frame -- Break!\n");
                   break;
               }
           } while (waitKey(30)<0); // while no key pressed
        }
    }
    else if (!TestImgDir(argv[2]))  // try opening it as a directory, but fail
    {  
       frame = imread(argv[2], CV_LOAD_IMAGE_COLOR); // try opening it as an image file
       if (frame.empty())  //not an image file
       {
	   		capture.open(argv[1]); // try opening it as a video file
           if (capture.isOpened())
           {   printf("video frame width x height: %f x %f\n", capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT));
               fps=0.0;
               do
               {   system_clock::time_point start = system_clock::now();
                   capture.read(frame);
                   //-- 3. Apply classifier to video frame
                   if( !frame.empty() )
                   {    //add fps to frame
                        char text[80];
                        sprintf(text,  "%2.1f", fps);
                        putText(frame, text, cvPoint(10,80), FONT_HERSHEY_PLAIN, 2, cvScalar(0,0,255),2,8);
                        detectAndDisplay(frame);
                        system_clock::time_point end = system_clock::now();
                        float seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                        fps = 1000000 / seconds;
                   }
                   else
                   {   printf("Warning: -- No frame -- Break!\n");
                       break;
                   }
               } while (waitKey(30)<0);
               return 0;
           }
           else
           {   
		   		printf("Could not open file: %s as video file\n", argv[1]);
               return -1;
           }
       }
       else // it is an image file
       {
	   		printf("image size: %d x %d\n", frame.cols,frame.rows);
           detectAndDisplay(frame);
		   waitKey(0);
       }
    }
    
    return 0;
 }

#if 1

void detectAndDisplay( Mat frame )
{
  std::vector<Rect> cars;
  Mat frame_gray;

 // cvtColor( frame, frame_gray, CV_BGR2GRAY );
  cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect cars
  car_cascade.detectMultiScale( frame_gray, cars, 1.1, 10, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
  

  for( size_t i = 0; i < cars.size(); i++ )
  {
    Point center( cars[i].x + cars[i].width*0.5, cars[i].y + cars[i].height*0.5 );
    ellipse( frame, center, Size( cars[i].width*0.5, cars[i].height*0.5), 0, 0, 360, Scalar( 0, 255, 0 ), 4, 8, 0 );

    Mat carROI = frame_gray( cars[i] );
    std::vector<Rect> faces;

    //-- In each car, detect faces
    face_cascade.detectMultiScale( carROI, faces, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );

    for( size_t j = 0; j < faces.size(); j++ )
     {
       Point center( cars[i].x + faces[j].x + faces[j].width*0.5, cars[i].y + faces[j].y + faces[j].height*0.5 );
       int radius = cvRound( (faces[j].width + faces[j].height)*0.25 );
       circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
     }
  }
  //-- Show what you got
  imshow( window_name, frame );
 }


#else

void detectAndDisplay( Mat frame )
{   std::vector<Rect> muzzles;
    Mat frame_gray;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //Detect dog muzzle area
    dog_cascade.detectMultiScale( frame_gray, muzzles, 1.1, 120, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50));
    for( size_t i = 0; i < muzzles.size(); i++ )
    {   Point center( muzzles[i].x + muzzles[i].width*0.5, muzzles[i].y + muzzles[i].height*0.5 );
        ellipse( frame, center, Size( muzzles[i].width*0.5, muzzles[i].height*0.5), 0, 0, 360, Scalar( 255, 255, 0 ), 4, 8, 0 );
    }
    char text[80];
    int dognumber = muzzles.size();
    if (dognumber>0) sprintf(text, "Found %d", dognumber);
    else sprintf(text,"0");
    putText(frame, text, cvPoint(10,30), FONT_HERSHEY_PLAIN, 2, cvScalar(0,0,255),2,8);
    if (testingImgDir==1)
    {   
		static int No;
		char filename[80];
        sprintf(filename,"Test%d.jpg",No++);
        imwrite(filename,frame);
    }
    else imshow(window_name,frame);
}
#endif

