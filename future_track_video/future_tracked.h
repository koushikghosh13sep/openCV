#ifndef FUTURE_TRACKED_H
#define FUTURE_TRACKED_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include <stdio.h>
#include <math.h>
#include<iostream>
#include <iomanip>
//#include"video_processor."

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



	void process(cv:: Mat &frame, cv:: Mat &output);
	// feature point detection
	void detectFeaturePoints();
	// determine if new points should be added
	bool addNewPoints();
	bool acceptTrackedPoint(int i);				// determine which tracked point should be accepted

	// handle the currently tracked points
	void handleTrackedPoints(cv:: Mat &frame,cv:: Mat &output);

};

#endif
