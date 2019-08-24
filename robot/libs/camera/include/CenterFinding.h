#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class CenterFinding{
	private:
		Mat sourceFrame;
		Mat outputFrame; 
		int max_kernel_size = 21;
		int max_kernel_length = 31;
		
		void cutImage();
		void toGrayScale();
		void useBlur();
		void erodeFrame(Mat element);
		void dilateFrame(Mat element);
		void thresholdFrame();
		vector<Point> findCenters();
		
	public:
		CenterFinding();
		CenterFinding(Mat frame);
		void setSourceFrame(Mat frame);
		Mat getSourceFrame();
		Mat getOutputFrame();
		vector<Point> findLineCenters();
		Mat drawPoints(vector<Point> centers);
		
};
