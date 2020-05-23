#include "bcm2835.h"
#include "l6470constants.h"
#include "motors.h"
#include <csignal>
#include <math.h>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <ctime>
#include <chrono>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "UdpJpgFrameStreamer.h"
#include "ContourFinding.h"
#include "CenterFinding.h"
using namespace cv;
using namespace std;

using boost::asio::ip::udp;

#define GPIO_TOF_1 	RPI_V2_GPIO_P1_12
#define GPIO_TOF_2 	RPI_V2_GPIO_P1_16
#define GPIO_TOF_3 	RPI_V2_GPIO_P1_18
#define GPIO_TOF_4 	RPI_V2_GPIO_P1_29
#define GPIO_TOF_5	RPI_V2_GPIO_P1_32
#define GPIO_TOF_6 	RPI_V2_GPIO_P1_31
#define GPIO_TOF_7 	RPI_V2_GPIO_P1_33
#define GPIO_TOF_8 	RPI_V2_GPIO_P1_35
#define GPIO_TOF_9 	RPI_V2_GPIO_P1_36
#define GPIO_TOF_10 RPI_V2_GPIO_P1_37
#define NDEBUG

void stepperTest();
Motors *globalBoard;
uint16_t measurement[10];
std::ofstream file;

void sigintHandler(int signum) {
	if (signum == SIGINT) {
		//globalBoard->Dump();
		file.close();
		globalBoard->stop();
		exit(signum);
	}
}


int main()
{
	signal(SIGINT, sigintHandler);

	if (bcm2835_init() == 0) {
			fprintf(stderr, "Not able to init the bmc2835 library\n");
			return -1;
	}

//	stepperTest();
	//-----------------------------------------------------

	int counter = 0;
	double total_prepare_time = 0;
	double total_finding_time = 0;
	double total_drawing_time = 0;
	int max_counter = 100;
	UdpJpgFrameStreamer streamer(2024, 64000, 80);
	double scale_factor = 0.5;
	ContourFinding contourFinder(1.0/6.0, 5.0/6.0);
	CenterFinding centerFinder(6);
	Mat src;
	std::cout<<"Contour or center finding? (1/2)";
	int method;
	std::cin>>method;
	//streamer.waitForClient();

	//odnośnik do kamery
	VideoCapture clipCapture(0);
	auto height = clipCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	auto width = clipCapture.get(CV_CAP_PROP_FRAME_WIDTH);
	clipCapture.set(CV_CAP_PROP_FRAME_HEIGHT, int(height*scale_factor));
  	clipCapture.set(CV_CAP_PROP_FRAME_WIDTH, int(width*scale_factor));
  	clipCapture.set(CV_CAP_PROP_CONVERT_RGB, false);

	//sprawdzenie czy wczytano poprawnie
	if (!clipCapture.isOpened())
	{
	  	cout  << "Could not open reference to clip" << endl;
		exit(0);
	}
	
	streamer.run();
	while(1){
		clipCapture.read(src);
			
		if (src.empty() || src.cols == -1 || src.rows == -1)
		{
		    	printf("No image data from clip\n");
		    	break;
		}

		else
		{	
			if(method == 1)
			{
				contourFinder.setFrame(src);
				contourFinder.setScaleFactor(scale_factor);//default is 0.5

				auto start = chrono::steady_clock::now(); 
				contourFinder.prepareImage();
				auto end = chrono::steady_clock::now();
				total_prepare_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now(); 
				std::vector<cv::Point> centers = contourFinder.findCenters();
				end = chrono::steady_clock::now();
				total_finding_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now();
				Mat frame = contourFinder.drawPoints(centers);
				end = chrono::steady_clock::now();
				total_drawing_time += chrono::duration_cast<chrono::microseconds>(end - start).count();
				++counter;

				if(counter == max_counter){
					cout << "Prepare time in microseconds: " << total_prepare_time/max_counter << " µs" << endl;
					cout << "Center finding time in microseconds: " << total_finding_time/max_counter << " µs" << endl;
					cout << "Drawing time in microseconds: " << total_drawing_time/max_counter << " µs" << endl;
					counter = 0;
					total_prepare_time = 0;
					total_finding_time = 0;
					total_drawing_time = 0;
				}

				streamer.pushFrame(frame);
			}

			else
			{
				centerFinder.setFrame(src);
				centerFinder.setScaleFactor(scale_factor);//default is 0.5

				auto start = chrono::steady_clock::now(); 
				centerFinder.prepareImage();
				auto end = chrono::steady_clock::now();
				total_prepare_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now();
				std::vector<cv::Point> centers = centerFinder.findCenters();
				end = chrono::steady_clock::now();
				total_finding_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now();
				Mat frame  = centerFinder.drawPoints(centers);
				end = chrono::steady_clock::now();
				total_drawing_time += chrono::duration_cast<chrono::microseconds>(end - start).count();
				++counter;

				if(counter == max_counter){
					cout << "Prepare time in microseconds: " << total_prepare_time/max_counter << " µs" << endl;
					cout << "Center finding time in microseconds: " << total_finding_time/max_counter << " µs" << endl;
					cout << "Drawing time in microseconds: " << total_drawing_time/max_counter << " µs" << endl;
					counter = 0;
					total_prepare_time = 0;
					total_finding_time = 0;
					total_drawing_time = 0;
				}

				streamer.pushFrame(frame);
			}
     	}


		//clipCapture.release();
		//break;
	}

  return 0;
}

void stepperTest(){

	long positionLeft,positionRight;
	Motors board( BCM2835_SPI_CS0, GPIO_RESET_OUT);
	globalBoard = &board;
	board.setUp();
	board.resetPosition();
	positionLeft = board.getPositionLeft();
	positionRight = board.getPositionRight();
	printf("Absolute position: Left:%lu		Right:%lu \n",positionLeft, positionRight);
	board.setSpeed(-20,-20);
	bcm2835_delay(500);
	positionLeft = board.getPositionLeft();
	positionRight = board.getPositionRight();
	printf("Absolute position: Left:%lu		Right:%lu \n",positionLeft, positionRight);
	file <<"end_left:"<< positionLeft <<"end_right:"<< positionRight<<std::endl;

	board.stop();

	file.close();
}
