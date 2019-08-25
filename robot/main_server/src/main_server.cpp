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

	UdpJpgFrameStreamer streamer;
	ContourFinding contourFinder;
	CenterFinding centerFinder(3);
	Mat src;
	std::cout<<"Contour or center finding? (1/2)";
	int method;
	std::cin>>method;
	while(1){
		streamer.waitForClient();

		//odnośnik do kamery
		VideoCapture clipCapture(0);

	   	//sprawdzenie czy wczytano poprawnie
	    if (!clipCapture.isOpened())
	    {
	  		cout  << "Could not open reference to clip" << endl;
	  		break;
	    }
    
		while(1){

			clipCapture.read(src);
			
		    	if (src.empty() || src.cols == -1 || src.rows == -1)
		    	{
		    	    printf("No image data from clip\n");
		    	    break;
		    	}
			
			if(method == 1)
			{
				contourFinder.setFrame(src);
				contourFinder.setScaleFactor(0.3);//default is 0.5

				auto start = chrono::steady_clock::now(); 
				std::vector<cv::Point> centers = contourFinder.findLineCenters();
				auto end = chrono::steady_clock::now();
				cout << "Center finding time in microseconds: " << chrono::duration_cast<chrono::microseconds>(end - start).count()
				<< " µs" << endl;
				Mat frame = contourFinder.drawPoints(centers);
				const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
				// Get the time offset in current day
    			const boost::posix_time::time_duration td = now.time_of_day();
				const long hours = td.hours();
    			const long minutes = td.minutes();
    			const long seconds = td.seconds();
    			const long milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
				char buf[40];
    			sprintf(buf, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, milliseconds);
    			cout<<"Send time in microseconds: "<<buf<<endl;
				streamer.pushFrame(frame);
			}

			else
			{
				centerFinder.setFrame(src);
				centerFinder.setScaleFactor(0.3);//default is 0.5

				auto start = chrono::steady_clock::now();
				std::vector<cv::Point> centers = centerFinder.findLineCenters();
				auto end = chrono::steady_clock::now();
				cout << "Center finding time in microseconds: " << chrono::duration_cast<chrono::microseconds>(end - start).count()
				<< " µs" << endl;
				Mat frame  = centerFinder.drawPoints(centers);
				const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
				// Get the time offset in current day
    			const boost::posix_time::time_duration td = now.time_of_day();
				const long hours = td.hours();
    			const long minutes = td.minutes();
    			const long seconds = td.seconds();
    			const long milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
				char buf[40];
    			sprintf(buf, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, milliseconds);
    			cout<<"Send time: "<<buf<<endl;
				streamer.pushFrame(frame);
			}
     	}

		clipCapture.release();
		break;
   
	}

  return 0;
}

void stepperTest(){

	file.open("odometry_log_report1000ms-200");

	long positionLeft,positionRight;
	int voltage;
	Motors board( BCM2835_SPI_CS0, GPIO_RESET_OUT);
	globalBoard = &board;
	board.setUp();
	board.resetPosition();
	positionLeft = board.getPositionLeft();
	positionRight = board.getPositionRight();
	//voltage = board.getBatteryVoltage();
	printf("Absolute position: Left:%lu		Right:%lu	Voltage:%d\n",positionLeft, positionRight,voltage);
	file <<"start_left:"<< positionLeft <<"start_right:"<< positionRight<<std::endl;
	board.setSpeed(200,200);
	bcm2835_delay(1000);
	positionLeft = board.getPositionLeft();
	positionRight = board.getPositionRight();
	//voltage = board.getBatteryVoltage();
	printf("Absolute position: Left:%lu		Right:%lu	Voltage:%d\n",positionLeft, positionRight,voltage);
	file <<"end_left:"<< positionLeft <<"end_right:"<< positionRight<<std::endl;
	bcm2835_delay(1000);

	board.stop();
	file.close();
}
