//finding rain drops from video file 

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<stdio.h>
#include<iostream>
#include <string>



static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/*** Helper function to display text in the center of a contour*/
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour){
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);



	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);

}


cv::Mat shapes(cv::Mat src,int fno){

	cv::Mat out,gb,bw;

	cv::cvtColor(src,out,CV_BGR2GRAY);
	cv::threshold(out,out,90,150,cv::THRESH_BINARY_INV);		
	cv::Canny(out,bw,90,150);					

	cv::imshow("s", bw);

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();

	for (int i = 0; i < contours.size(); i++){
		// Detect and label circles
		double area = cv::contourArea(contours[i]);
		cv::Rect r = cv::boundingRect(contours[i]);
		int radius = r.width / 2;



		if (std::abs(1 - ((double)r.width / r.height)) <= 0.9 && std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.9){
			setLabel(dst, "CIR", contours[i]);
			cv::Point pt(r.x + (r.width / 2), r.y + (r.height / 2));

			cv::circle(dst,pt, // circle centre
				radius,// circle radius
				cv::Scalar(255,0,0), // color
				5);// thickness
			printf("%d %d - %d\n",pt.x,pt.y,fno);

		}
	}
	return dst;
}

int main(int argc, char* argv[]){
	cv::VideoCapture input_cap("2stock-footage-raindrops-in-a-puddle.webm");
	if (!input_cap.isOpened()){
		std::cout << "!!! Input video could not be opened" << std::endl;
		return -1;
	}

	cv::VideoWriter output_cap;
	output_cap.open("output.avi",  
		(int)CV_FOURCC('X','V','I','D'),
		input_cap.get(CV_CAP_PROP_FPS), 
		cv::Size(input_cap.get(CV_CAP_PROP_FRAME_WIDTH),
		input_cap.get(CV_CAP_PROP_FRAME_HEIGHT)),
		true);

	if (!output_cap.isOpened()){
		std::cout << "!!! Output video could not be opened" << std::endl;
		return -1;
	}

	// Loop to read frames from the input capture and write it to the output capture
	cv::Mat frame;
	cv::Mat can;
	int fno=0;
	while (true){
		if (!input_cap.read(frame))             
			break;
		fno++;
		can=shapes(frame,fno);
		cv::imshow("abc", can);
		output_cap.write(can);

		output_cap.write(frame);
		cv::waitKey(48);
	}
	input_cap.release();
	return 0;
}
