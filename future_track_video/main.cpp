#include "processor.h"
#include "future_tracked.h"

int main( int argc, char** argv ){
	char* videoName = argv[1];
	char* outputVideoName = argv[2];

	VideoProcessor processor;		// Create video procesor instance
	FeatureTracker tracker;			// Create feature tracker instance

	processor.setInput(videoName);	// Open video file
	processor.setOutput(outputVideoName,0, 0.0,true);

	processor.setFrameProcessor(&tracker);	// set frame processor
//processor.setFrameProcessor(&framePreocessor);
	processor.displayOutput("Tracked Features");// Declare a window to display the video
	processor.setDelay(1000./processor.getFrameRate());// Play the video at the original frame rate
	processor.run();			// Start the process
}

