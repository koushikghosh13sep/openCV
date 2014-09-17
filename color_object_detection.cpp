

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>

using namespace cv;
using namespace std;

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;


void drawObject(int x, int y, Mat &frame){
	cv::circle(frame,Point(x,y),10,cv::Scalar(0,0,255));
}

void on_trackbar( int value, void* userdata) {
	H_MIN = H_MIN;
	H_MAX = H_MAX;
	S_MIN = S_MIN;
	S_MAX = S_MAX;
	V_MIN = V_MIN;
	V_MAX = V_MAX;

}
void createTrackbars(){


	namedWindow("trackbars",0);
	char TrackbarName[50];

	sprintf(TrackbarName,"H_MIN %d",H_MAX);
	sprintf(TrackbarName,"H_MAX %d",H_MAX);
	sprintf(TrackbarName,"S_MIN %d",S_MIN);
	sprintf(TrackbarName,"S_MAX %d",S_MAX);
	sprintf(TrackbarName,"V_MIN %d",V_MIN);
	sprintf(TrackbarName,"V_MAX %d",V_MAX);


	createTrackbar("H_MIN", "trackbars", &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", "trackbars", &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", "trackbars", &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", "trackbars", &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", "trackbars", &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", "trackbars", &V_MAX, V_MAX, on_trackbar);

}


void morphOps(Mat &thresh){
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(1,1));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(2,2));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}


void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){
	Mat temp;
	threshold.copyTo(temp);
	vector<vector<Point> >contours;
	vector<Vec4i> hierarchy;
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	
	double refArea = 0;
	bool objectFound = false;
	if(hierarchy.size() > 0){
		int numObjects = hierarchy.size();
		if(numObjects < 10){
			for( int index = 0; index >= 0;index = hierarchy[index][0]){
				Moments moment = moments((cv::Mat)contours[index]);

				double area = moment.m00;

				if(area > 400 && area<FRAME_HEIGHT*FRAME_WIDTH/1.5 && area > refArea){
					printf("%f\n",area);
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
				}else objectFound = false;
			}
		
			if(objectFound == true){
				putText(cameraFeed,"tracking object", Point(0,50), 2, 1, Scalar(0,255,0),2);
				drawObject(x,y,cameraFeed);
			}
		}else putText(cameraFeed,"Too Much Noise",Point(0,50), 2, 1, Scalar(0,0,255), 2);
	}
}



int main(int argc, const char * argv[]) {
	bool trackObject = true;
	bool useMorphOps = true;

	Mat cameraFeed;
	Mat HSV;
	Mat threshold;

	int x = 0, y = 0;
	createTrackbars();

	Mat img = imread(argv[1]);

	resize(img, img, Size(), 4, 4, INTER_LINEAR);
	Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 9, 9 ), Point(3, 3) );
	dilate( img, img, element );

	while(1){
//		capture.read(cameraFeed);
		cvtColor(img,HSV,COLOR_BGR2HSV);
		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX), threshold);



		if(useMorphOps)
			morphOps(threshold);
		if(trackObject)
			trackFilteredObject(x,y,threshold,cameraFeed);
			


// on_trackbar( H_MAX, 0 );

		imshow("windowName",img);
		imshow("windowName1",HSV);
		imshow("windowName2",threshold);
		waitKey(100);	
	}
	return 0;
}
