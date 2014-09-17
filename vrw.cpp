//Canny edge from video and save canny video 
 
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<stdio.h>
#include<iostream>
#include <string>


void canny(cv::Mat& img, cv::Mat& out) {

	cv::cvtColor(img,out,CV_BGR2GRAY);
	// Compute Canny edges
	cv::threshold(out,out,85,145,cv::THRESH_BINARY_INV);
	cv::Canny(out,out,85,145);
}





int main(int argc, char* argv[]){
	// Load input video
	cv::VideoCapture input_cap(argv[1]);
	if (!input_cap.isOpened()){
		std::cout << "!!! Input video could not be opened" << std::endl;
		return -1;
	}

	// Setup output video
	cv::VideoWriter output_cap;
	output_cap.open("output.avi",  
		(int)CV_FOURCC('X','V','I','D'),
		input_cap.get(CV_CAP_PROP_FPS), 
		cv::Size(input_cap.get(CV_CAP_PROP_FRAME_WIDTH), input_cap.get(CV_CAP_PROP_FRAME_HEIGHT)),false); //true for color image
														// false for gray image

	if (!output_cap.isOpened()){
		std::cout << "!!! Output video could not be opened" << std::endl;
		return -1;
	}

	// Loop to read frames from the input capture and write it to the output capture
	cv::Mat frame;
	cv::Mat can;
	while (true){
		if (!input_cap.read(frame))             
			break;
		canny(frame,can);
		output_cap.write(frame);
	        output_cap.write(frame);
	}

	input_cap.release();
	output_cap.release();

	return 0;
}
