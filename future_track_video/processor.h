#ifndef VIDEO_PROCESSOR_H
#define VIDEO_PROCESSOR_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "future_tracked.h"

using namespace cv;
using namespace std;


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
	VideoProcessor() : callIt(true), delay(0), fnumber(0), frameToStop(-1), stop(false) {}


	double getFrameRate();
	
	cv::Size getFrameSize();

	// set the name of the video file
	bool setInput(std::string filename);
	// to display the processed frames
	void displayInput(std::string wn);
	// to display the processed frames
	void displayOutput(std::string wn);
	// do not display the processed frames
	void dontDisplay();
	// set the callback function that
	// will be called for each frame
	void setFrameProcessor(FrameProcessor* frameProcessorPtr);
	// set the callback function that will
	// be called for each frame
	void setFrameProcessor(	void (*frameProcessingCallback)(cv::Mat&, cv::Mat&));
	void stopIt();			// Stop the processing
	bool isStopped();		// Is the process stopped?
	bool isOpened();		// Is a capture device opened?
	// set a delay between each frame
	// 0 means wait at each frame
	// negative means no delay
	void setDelay(int d);
	// to get the next frame
	// could be: video file or camera
	bool readNextFrame(cv::Mat& frame);
	// process callback to be called
	void callProcess();
	// do not call process callback
	void dontCallProcess();

	void stopAtFrameNo(long frame);
	
	// return the frame number of the next frame
	long getFrameNumber();

	// set the output as a series of image files
	// extension must be ".jpg", ".bmp" ...
	bool setOutput(const std::string &filename, const std::string &ext, int numberOfDigits, int startIndex);
	bool setOutput(const std::string &filename, int codec, double framerate,bool isColor);
	// to write the output frame
	// could be: video file or images
	void writeNextFrame(cv::Mat& frame);
	void run();
	// get the codec of input video
	int getCodec(char codec[4]);
	
};
#endif
