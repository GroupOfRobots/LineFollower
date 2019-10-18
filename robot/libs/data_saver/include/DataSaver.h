#include <iostream>
#include <string>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <fstream>
using namespace cv;
using namespace std;

class DataSaver {
	private:
		string pathToSaveData;
		int frameWidth;
		int frameHeight;
		VideoWriter videoWriter;
		ofstream txtWriter;
		
	public:
		DataSaver(string txtFileName, string recordingFileName, string pathToSaveData, int frameWidth, int frameHeight, int frameInterval);
		~DataSaver();
		void setFrame(Mat frame);
		void setDataToTxt(int leftMotor, int rightMotor, int setPoint, int error, int exec_duration);
};

