#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class ContourFinding{
	private:
		double pointFactorToStartCutting;
		double pointFactorToFinishCutting;
		Point pointToStartCutting;
		Point pointToFinishCutting;
		Mat sourceFrame;
		Mat outputFrame; 
		int max_kernel_size = 21;
		int max_kernel_length = 31;
		double scaleFactor = 0.5;
		vector<Point> contour;
		vector<Vec4i> hierarchy;
		
		void scaleImage();
		void cutImage();
		void toGrayScale();
		void useBlur();
		void erodeFrame(Mat element);
		void dilateFrame(Mat element);
		void thresholdFrame();
		
	public:
		ContourFinding(double pointFactorToStartCutting, double pointFactorToFinishCutting);
		ContourFinding(Mat frame, double pointFactorToStartCutting, double pointFactorToFinishCutting);
		void setFrame(Mat frame);
		void setScaleFactor(double scaleFactor);
		double getScaleFactor();
		Mat getSourceFrame();
		Mat getOutputFrame();
		void prepareImage();
		vector<Point> findCenters();
		Mat drawPoints(vector<Point> centers);
		
};
