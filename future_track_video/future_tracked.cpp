/*
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include <stdio.h>
#include <math.h>
#include<iostream>
#include <iomanip>
*/
#include"future_tracked.h"

using namespace cv;
using namespace std;



void FeatureTracker::process(cv:: Mat &frame, cv:: Mat &output) {

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
void FeatureTracker::detectFeaturePoints() {
	// detect the features
	cv::goodFeaturesToTrack(gray, // the image
		features,// the output detected features
		max_count, // the maximum number of features
		qlevel, // quality level
		minDist); // min distance between two features
}

// determine if new points should be added
bool FeatureTracker::addNewPoints() {
	return points[0].size()<=10;				// if too few points
}

bool FeatureTracker::acceptTrackedPoint(int i) {				// determine which tracked point should be accepted
		// if point has moved			
		return status[i] &&(abs(points[0][i].x-points[1][i].x)+(abs(points[0][i].y-points[1][i].y))>2);
}

	// handle the currently tracked points
void FeatureTracker::handleTrackedPoints(cv:: Mat &frame,cv:: Mat &output) {
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
