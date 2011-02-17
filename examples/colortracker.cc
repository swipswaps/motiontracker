#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <boost/lexical_cast.hpp>
#include "motiontracker.hh"
#include "utils.hh"

using namespace cv;

struct MyTracker: public WebcamListener, public MotionListener {
	std::string window;
	FPSCounter counter;
	volatile int x;
	volatile int y;

	MyTracker(Webcam& cam, MotionTracker& tr, std::string win)
		: WebcamListener(cam), MotionListener(tr), window(win), counter(5), x(), y()
	{ }

	void motionEvent(cv::Vec3f pos, cv::Vec3f) {
		x = pos[0];
		y = pos[1];
	}

	void frameEvent(const cv::Mat& frame) {
		Mat img(frame);
		// Add FPS indicator
		putText(img, boost::lexical_cast<std::string>(counter.getFPS()),
			Point(x, y), FONT_HERSHEY_PLAIN, 2, CV_RGB(255,0,255));
		// Show on screen
		imshow(window, img);
		counter(); // Update FPS counter
	}
};

int main(int argc, char** argv)
{
	(void)argc; (void)argv; // Suppress warnings
	boost::scoped_ptr<Webcam> webcam;
	boost::scoped_ptr<ColorTracker> tracker;
	try {
		webcam.reset(new Webcam);
		tracker.reset(new ColorTracker(*webcam, 100)); // Track darkish blue
	} catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	std::string window = "video";
	namedWindow(window, 1);

	{
		// Launch a receiver for doing the work whenever a frame is available
		MyTracker mytracker(*webcam, *tracker, window);

		// Rest here
		while (waitKey(30) < 0);
	}

	cvDestroyWindow(window.c_str());
	return 0;
}
