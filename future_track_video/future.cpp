#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include <stdio.h>
#include <math.h>
#include<iostream>
#include <iomanip>
//#include"video_processor.cpp"

using namespace cv;
using namespace std;



// The frame processor interface
class FrameProcessor {
	public:
	// processing method
	virtual void process(cv:: Mat &input, cv:: Mat &output)= 0;


	// set the instance of the class that
	// implements the FrameProcessor interface

};


class FeatureTracker : public FrameProcessor{
	cv::Mat gray;					// current gray-level image
	cv::Mat gray_prev;				// previous gray-level image
	std::vector<cv::Point2f> points[2];		// tracked features from 0->1
	std::vector<cv::Point2f> initial;		// initial position of tracked points
	std::vector<cv::Point2f> features;		// detected features
	int max_count;					// maximum number of features to detect
	double qlevel;					// quality level for feature detection
	double minDist;					// min distance between two points
	std::vector<uchar> status;			// status of tracked features
	std::vector<float> err;				// error in tracking

	public:
		FeatureTracker() : max_count(500), qlevel(0.01), minDist(10.) {}



/*
void process(cv::Mat& img, cv::Mat& out) {
	// Convert to gray
	if (img.channels()==3)
		cv::cvtColor(img,out,CV_BGR2GRAY);
	// Compute Canny edges
//	cv::Canny(out,out,100,200);
	// Invert the image
//	cv::threshold(out,out,128,255,cv::THRESH_BINARY_INV);
}
*/


		void process(cv:: Mat &frame, cv:: Mat &output) {

			cv::cvtColor(frame, gray, CV_BGR2GRAY);			// convert to gray-level image
			frame.copyTo(output);

			if(addNewPoints()){					// 1. if new feature points must be added
				detectFeaturePoints();				// detect feature points
				// add the detected features to
				// the currently tracked features
				points[0].insert(points[0].end(),features.begin(),features.end());
				initial.insert(initial.end(),features.begin(),features.end());
			}


			if(gray_prev.empty())					// for first image of the sequence
				gray.copyTo(gray_prev);

			// 2. track features
			cv::calcOpticalFlowPyrLK(
				gray_prev, gray, // 2 consecutive images
				points[0], // input point positions in first image
				points[1], // output point positions in the 2nd image
				status,	// tracking success
				err);


			// tracking error
			// 2. loop over the tracked points to reject some
			int k=0;
			for( int i= 0; i < points[1].size(); i++ ) {
				// do we keep this point?
				if (acceptTrackedPoint(i)) {
					// keep this point in vector
					initial[k]= initial[i];
					points[1][k++] = points[1][i];
				}
			}
			// eliminate unsuccesful points
			points[1].resize(k);
			initial.resize(k);

			handleTrackedPoints(frame, output);			// 3. handle the accepted tracked points
			std::swap(points[1], points[0]);			// 4. current points and image become previous ones
			cv::swap(gray_prev, gray);
		}

		// feature point detection
		void detectFeaturePoints() {
			// detect the features
			cv::goodFeaturesToTrack(gray, // the image
				features,// the output detected features
				max_count, // the maximum number of features
				qlevel, // quality level
				minDist); // min distance between two features
		}
		// determine if new points should be added
		bool addNewPoints() {
			return points[0].size()<=10;				// if too few points
		}


		bool acceptTrackedPoint(int i) {				// determine which tracked point should be accepted
			// if point has moved			
			return status[i] &&(abs(points[0][i].x-points[1][i].x)+(abs(points[0][i].y-points[1][i].y))>2);
		}

		// handle the currently tracked points
		void handleTrackedPoints(cv:: Mat &frame,cv:: Mat &output) {
			// for all tracked points
			for(int i= 0; i < points[1].size(); i++ ) {
				// draw line and circle
				cv::line(output,
					initial[i], // initial position
					points[1][i],// new position
					cv::Scalar(255,255,255));
				cv::circle(output, points[1][i], 3,cv::Scalar(255,255,255),-1);
			}
		}
};




class VideoProcessor {

	cv::VideoCapture capture;		// the OpenCV video capture object
	void (*process)(cv::Mat&, cv::Mat&);	// the callback function to be called for the processing of each frame
	bool callIt;				// a bool to determine if the process callback will be called
	std::string windowNameInput;		// Input display window name
	std::string windowNameOutput;		// Output display window name
	int delay;				// delay between each frame processing
	long fnumber;				// number of processed frames
	long frameToStop;			// stop at this frame number
	bool stop;				// to stop the processing

	cv::VideoWriter writer;			// the OpenCV video writer object

	std::string outputFile;			// output filename
	int currentIndex;			// current index for output images
	int digits;				// number of digits in output image filename
	std::string extension;			// extension of output images
	double framerate;

FrameProcessor *frameProcessor;

	public:
	VideoProcessor() : callIt(true), delay(0), fnumber(0), stop(false), frameToStop(-1) {}


	double getFrameRate(){
		return capture.get(CV_CAP_PROP_FPS);
	}
	
//	cv::cvSize getFrameSize(){
//		return cvSize(640, 480);
//	}

	// set the name of the video file
	bool setInput(std::string filename) {
		fnumber= 0;
	// In case a resource was already
	// associated with the VideoCapture instance
		capture.release();
//		images.clear();
	// Open the video file
		return capture.open(filename);
	}

	// to display the processed frames
	void displayInput(std::string wn) {
		windowNameInput= wn;
		cv::namedWindow(windowNameInput);
	}
	// to display the processed frames
	void displayOutput(std::string wn) {
		windowNameOutput= wn;
		cv::namedWindow(windowNameOutput);
	}
	// do not display the processed frames
	void dontDisplay() {
		cv::destroyWindow(windowNameInput);
		cv::destroyWindow(windowNameOutput);
		windowNameInput.clear();
		windowNameOutput.clear();
	}

	// set the callback function that
	// will be called for each frame
//void setFrameProcessor( void (*frameProcessingCallback)(cv::Mat&, cv::Mat&)) {
//	process = frameProcessingCallback;
//}

	void setFrameProcessor(FrameProcessor* frameProcessorPtr){
		// invalidate callback function
		process = 0;
		// this is the frame processor instance
		// that will be called
		frameProcessor= frameProcessorPtr;
		callProcess();
	}
	// set the callback function that will
	// be called for each frame
	void setFrameProcessor(	void (*frameProcessingCallback)(cv::Mat&, cv::Mat&)) {
		// invalidate frame processor class instance
		frameProcessor= 0;
		// this is the frame processor function that
		// will be called
		process= frameProcessingCallback;
		callProcess();
	}



	void stopIt() {			// Stop the processing
		stop= true;
	}

	bool isStopped() {		// Is the process stopped?
		return stop;
	}

	bool isOpened() {		// Is a capture device opened?
		capture.isOpened();
	}
	// set a delay between each frame
	// 0 means wait at each frame
	// negative means no delay
	void setDelay(int d) {
		
		delay= d;
	}
	// to get the next frame
	// could be: video file or camera
	bool readNextFrame(cv::Mat& frame){
		return capture.read(frame);
	}
	// process callback to be called
	void callProcess() {
		callIt= true;
	}
	// do not call process callback
	void dontCallProcess() {
		callIt= false;
	}

	void stopAtFrameNo(long frame) {
		frameToStop= frame;
	}
	
	// return the frame number of the next frame
	long getFrameNumber() {
		// get info of from the capture device
		long fnumber= static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));
		return fnumber;
	}

	// set the output as a series of image files
	// extension must be ".jpg", ".bmp" ...
	bool setOutput(const std::string &filename, const std::string &ext, int numberOfDigits=3, int startIndex=0) {
		// start index
		// number of digits must be positive
		if (numberOfDigits<0)
			return false;
			// filenames and their common extension
		outputFile = filename;
		extension= ext;
		// number of digits in the file numbering scheme
		digits= numberOfDigits;
		// start numbering at this index
		currentIndex= startIndex;
		return true;
	}

	bool setOutput(const std::string &filename, int codec=0, double framerate=0.0,bool isColor=true) {
printf("cccccccccccccc\n");		
	outputFile= filename;
		extension.clear();
		if (framerate==0.0)
			framerate= getFrameRate(); // same as input
	
		char c[4];
		if (codec==0){
			codec= getCodec(c);	// use same codec as input
		}
		// Open output video
//		return writer.open(outputFile, codec, framerate, getFrameSize(), isColor);
		return writer.open(outputFile, codec, framerate, cvSize(640, 480), isColor);

	}

	// to write the output frame
	// could be: video file or images
	void writeNextFrame(cv::Mat& frame) {
		if (extension.length()) { // then we write images
printf("aaaaaaaa\n");
			std::stringstream ss;
			// compose the output filename
			ss << outputFile << std::setfill('0')<< std::setw(digits)<< currentIndex++ << extension;
			cv::imwrite(ss.str(),frame);
		} else { 
printf("bbbbbbbb\n");
			writer.write(frame);		// then write to video file
		}
	}


	void run(){
	
		cv::Mat frame;
		cv::Mat output;

		if (!isOpened())		// if no capture device has been set
			return;
		stop= false;


		while (!isStopped()) {
			if (!readNextFrame(frame))				// read next frame if any
				break;

			if (windowNameInput.length()!=0)			// display input frame
				cv::imshow(windowNameInput,frame);
	
			if (callIt) {						// calling the process function or method
				if (process){					// process the frame
					process(frame, output);
				}
				else if (frameProcessor){
					frameProcessor->process(frame,output);
				}
				fnumber++;					// increment frame number
			} else {
				output= frame;
			}
			if (outputFile.length()!=0)
				writeNextFrame(output);

			if (windowNameOutput.length()!=0)			// display output frame
				cv::imshow(windowNameOutput,output);
			if (delay>=0 && cv::waitKey(delay)>=0)			// introduce a delay
				stopIt();
	
			if (frameToStop>=0 && getFrameNumber()==frameToStop)// check if we should stop
				stopIt();
		}
	}

	// get the codec of input video
	int getCodec(char codec[4]) {
		// undefined for vector of images
//		if (images.size()!=0) 
//			return -1;
		union { // data structure for the 4-char code
			int value;
			char code[4]; 
		}returned;
			// get the code
		returned.value= static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
		// get the 4 characters
		codec[0]= returned.code[0];
		codec[1]= returned.code[1];
		codec[2]= returned.code[2];
		codec[3]= returned.code[3];
		return returned.value;				// return the int value corresponding to the code
	}
};




int main(){

	VideoProcessor processor;		// Create video procesor instance
	FeatureTracker tracker;			// Create feature tracker instance
//FrameProcessor framePreocessor;

	processor.setInput("../TJN.avi");	// Open video file
	processor.setFrameProcessor(&tracker);	// set frame processor
//processor.setFrameProcessor(&framePreocessor);
	processor.displayOutput("Tracked Features");// Declare a window to display the video
	processor.setDelay(1000./processor.getFrameRate());// Play the video at the original frame rate
	processor.run();			// Start the process
}

