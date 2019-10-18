#include "DataSaver.h"

DataSaver::DataSaver(string txtFileName, string recordingFileName, string pathToSaveData, int frameWidth, int frameHeight, int frameInterval = 60000){
	this->pathToSaveData = pathToSaveData;
	this->frameHeight = frameHeight;
	this->frameWidth = frameWidth;
	frameInterval = int(round(frameInterval/1000000));//conversion from microseconds to seconds
	this->videoWriter = VideoWriter(pathToSaveData + "/" + recordingFileName + ".avi", cv::VideoWriter::fourcc('M','J','P','G'), frameInterval, Size(frameWidth, frameHeight));
	txtWriter.open(pathToSaveData + "/" + txtFileName + ".txt");
	start = chrono::steady_clock::now();
}

DataSaver::~DataSaver(){
	this->videoWriter.release();
	this->txtWriter.close();
}

void DataSaver::setFrame(Mat frame){
	if (!frame.empty()) videoWriter.write(frame);
}

void DataSaver::setDataToTxt(int leftMotor, int rightMotor, int setPoint, int error, int exec_duration, double timestamp){
	txtWriter << leftMotor << "," << rightMotor << "," << setPoint << "," << error << "," << exec_duration << "," << timestamp << endl;
}

void DataSaver::setData(Mat frame, int leftMotor, int rightMotor, int setPoint, int error, int exec_duration){
	setFrame(frame);
	auto end = chrono::steady_clock::now();
	double timestamp = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	setDataToTxt(leftMotor, rightMotor, setPoint, error, exec_duration, timestamp);
}