//crop 60x60 from a src image and save 
//left mouse click to select 
//enter for save

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <iomanip> 

using namespace cv;
using namespace std;

cv::Rect selection;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if ( event == EVENT_LBUTTONDOWN )
	{
		std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		selection = Rect(x-30,y-30,60,60);

	}else if ( event == EVENT_MOUSEMOVE )
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

int main( int argc, char** argv ){
	char* imageName = argv[1];
	char *outputFile=argv[2];

	cv::Mat image = imread( imageName); 

	namedWindow("My Window", 1);
	setMouseCallback("My Window", CallBackFunc, NULL);
	imshow("My Window", image);

	waitKey(0);

	cv::Mat croppedImage = image(selection);
	imshow("My Window", croppedImage);

	waitKey(0);

	imwrite( outputFile, croppedImage );

}
