#include "processor.h"

double VideoProcessor::getFrameRate(){
		return capture.get(CV_CAP_PROP_FPS);
}
	
Size VideoProcessor::getFrameSize(){
		return cv::Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	}

	// set the name of the video file
bool VideoProcessor::setInput(std::string filename) {
	fnumber= 0;
// In case a resource was already
// associated with the VideoCapture instance
	capture.release();
//	images.clear();
// Open the video file
	return capture.open(filename);
}

// to display the processed frames
void VideoProcessor::displayInput(std::string wn) {
	windowNameInput= wn;
	cv::namedWindow(windowNameInput);
}

// to display the processed frames
void VideoProcessor::displayOutput(std::string wn) {
	windowNameOutput= wn;
	cv::namedWindow(windowNameOutput);
}

// do not display the processed frames
void VideoProcessor::dontDisplay() {
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

void VideoProcessor::setFrameProcessor(FrameProcessor* frameProcessorPtr){
	// invalidate callback function
	process = 0;
	// this is the frame processor instance
	// that will be called
	frameProcessor= frameProcessorPtr;
	callProcess();
}

// set the callback function that will
// be called for each frame
void VideoProcessor::setFrameProcessor(	void (*frameProcessingCallback)(cv::Mat&, cv::Mat&)) {
	// invalidate frame processor class instance
	frameProcessor= 0;
	// this is the frame processor function that
	// will be called
	process= frameProcessingCallback;
	callProcess();
}



void VideoProcessor::stopIt() {			// Stop the processing
	stop= true;
}

bool VideoProcessor::isStopped() {		// Is the process stopped?
	return stop;
}

bool VideoProcessor::isOpened() {		// Is a capture device opened?
	return capture.isOpened();
}
// set a delay between each frame
// 0 means wait at each frame
// negative means no delay
void VideoProcessor::setDelay(int d) {
	delay= d;
}

// to get the next frame
// could be: video file or camera
bool VideoProcessor::readNextFrame(cv::Mat& frame){
	return capture.read(frame);
}

// process callback to be called
void VideoProcessor::callProcess() {
	callIt= true;
}
// do not call process callback
void VideoProcessor::dontCallProcess() {
	callIt= false;
}

void VideoProcessor::stopAtFrameNo(long frame) {
	frameToStop= frame;
}
	
// return the frame number of the next frame
long VideoProcessor::getFrameNumber() {
	// get info of from the capture device
	long fnumber= static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));
	return fnumber;
}

// set the output as a series of image files
// extension must be ".jpg", ".bmp" ...
bool VideoProcessor::setOutput(const std::string &filename, const std::string &ext, int numberOfDigits=3, int startIndex=0) {
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

bool VideoProcessor::setOutput(const std::string &filename, int codec=0, double framerate=0.0,bool isColor=true) {
	outputFile= filename;
//	extension.clear();
	if (framerate==0.0)
		framerate= getFrameRate(); // same as input
	
	char c[4];
	if (codec==0){
		codec= getCodec(c);	// use same codec as input
	}
printf("AAAAAAAAAAAAAAA\n");
	// Open output video
	return writer.open("outputFile.avi", codec, framerate, getFrameSize(), isColor);
}

// to write the output frame
// could be: video file or images
void VideoProcessor::writeNextFrame(cv::Mat& frame) {
	if (extension.length()) { // then we write images
		std::stringstream ss;
		// compose the output filename
		ss << outputFile << std::setfill('0')<< std::setw(digits)<< currentIndex++ << extension;
		cv::imwrite(ss.str(),frame);
	} else { 
		writer.write(frame);		// then write to video file
	}
}


void VideoProcessor::run(){
	
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
int VideoProcessor::getCodec(char codec[4]) {
	// undefined for vector of images
//	if (images.size()!=0) 
//		return -1;
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

