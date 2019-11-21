#include "bcm2835.h"
#include "l6470constants.h"
#include "motors.h"
#include "VL53L1X.h"
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
void tofTest();
Motors *globalBoard;
VL53L1X *globalSensors[10];
uint16_t measurement[10];
std::ofstream file;

void sigintHandler(int signum) {
	if (signum == SIGINT) {
		//globalBoard->Dump();
		file.close();
		if (globalBoard != nullptr) globalBoard->stop();
		for(int i = 0; i < 10; i++)
			if (globalSensors[i] != nullptr) globalSensors[i]->disable();
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

	tofTest();

//	stepperTest();
	//-----------------------------------------------------
/*
	int counter = 0;
	double total_center_time = 0;
	double total_drawing_time = 0;
	int max_counter = 1000;
	UdpJpgFrameStreamer streamer(2024, 64000, 80);
	ContourFinding contourFinder(1.0/3.0, 2.0/3.0);
	CenterFinding centerFinder(6);
	Mat src;
	std::cout<<"Contour or center finding? (1/2)";
	int method;
	std::cin>>method;
	//streamer.waitForClient();

	//odnośnik do kamery
	VideoCapture clipCapture(0);

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
				contourFinder.setScaleFactor(0.3);//default is 0.5

				auto start = chrono::steady_clock::now(); 
				std::vector<cv::Point> centers = contourFinder.findLineCenters();
				auto end = chrono::steady_clock::now();
				total_center_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now();
				Mat frame = contourFinder.drawPoints(centers);
				end = chrono::steady_clock::now();
				total_drawing_time += chrono::duration_cast<chrono::microseconds>(end - start).count();
				++counter;

				if(counter == max_counter){
					cout << "Center finding time in microseconds: " << total_center_time/max_counter << " µs" << endl;
					cout << "Drawing time in microseconds: " << total_drawing_time/max_counter << " µs" << endl;
					counter = 0;
					total_drawing_time = 0;
					total_center_time = 0;
				}

				streamer.pushFrame(frame);
			}

			else
			{
				centerFinder.setFrame(src);
				centerFinder.setScaleFactor(0.5);//default is 0.5

				auto start = chrono::steady_clock::now();
				std::vector<cv::Point> centers = centerFinder.findLineCenters();
				auto end = chrono::steady_clock::now();
				total_center_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

				start = chrono::steady_clock::now();
				Mat frame  = centerFinder.drawPoints(centers);
				end = chrono::steady_clock::now();
				total_drawing_time += chrono::duration_cast<chrono::microseconds>(end - start).count();
				++counter;

				if(counter == max_counter){
					cout << "Center finding time in microseconds: " << total_center_time/max_counter << " µs" << endl;
					cout << "Drawing time in microseconds: " << total_drawing_time/max_counter << " µs" << endl;
					counter = 0;
					total_drawing_time = 0;
					total_center_time = 0;
				}

				streamer.pushFrame(frame);
			}
     	}


		//clipCapture.release();
		//break;
	}*/

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

void tofTest(){

	// Log file
	file.open("distance_log_report70cm");

	bcm2835_gpio_fsel(GPIO_TOF_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_5, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_6, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_7, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_8, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_9, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(GPIO_TOF_10, BCM2835_GPIO_FSEL_OUTP);

	//disable all sensors first
	bcm2835_gpio_clr(GPIO_TOF_1); // górny
	bcm2835_gpio_clr(GPIO_TOF_2); // górny
	bcm2835_gpio_clr(GPIO_TOF_3);
	bcm2835_gpio_clr(GPIO_TOF_4);
	bcm2835_gpio_clr(GPIO_TOF_5);
	bcm2835_gpio_clr(GPIO_TOF_6);
	bcm2835_gpio_clr(GPIO_TOF_7);
	bcm2835_gpio_clr(GPIO_TOF_8);
	bcm2835_gpio_clr(GPIO_TOF_9);
	bcm2835_gpio_clr(GPIO_TOF_10);

	bcm2835_i2c_begin(); //begin I2C
	bcm2835_i2c_set_baudrate(40000);

	//enable sensor one and change address
	bcm2835_gpio_set(GPIO_TOF_1);
	delay(10);
	globalSensors[0] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[0]->setAddress(0x30);
	delay(10);
	puts("Sensor one started at: 0x30");

	bcm2835_gpio_set(GPIO_TOF_2);
	delay(10);
	globalSensors[1] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[1]->setAddress(0x31);
	delay(10);
	puts("Sensor two started at: 0x31");

	bcm2835_gpio_set(GPIO_TOF_3);
	delay(10);
	globalSensors[2] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[2]->setAddress(0x32);
	delay(10);
	puts("Sensor three started at: 0x32");

	bcm2835_gpio_set(GPIO_TOF_4);
	delay(10);
	globalSensors[3] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[3]->setAddress(0x33);
	delay(10);
	puts("Sensor four started at: 0x33");

	bcm2835_gpio_set(GPIO_TOF_5);
	delay(10);
	globalSensors[4] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[4]->setAddress(0x34);
	delay(10);
	puts("Sensor five started at: 0x34");

	bcm2835_gpio_set(GPIO_TOF_6);
	delay(10);
	globalSensors[5] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[5]->setAddress(0x35);
	delay(10);
	puts("Sensor six started at: 0x35");

	bcm2835_gpio_set(GPIO_TOF_7);
	delay(10);
	globalSensors[6] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[6]->setAddress(0x36);
	delay(10);
	puts("Sensor seven started at: 0x36");

	bcm2835_gpio_set(GPIO_TOF_8);
	delay(10);
	globalSensors[7] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[7]->setAddress(0x37);
	delay(10);
	puts("Sensor eight started at: 0x37");

	bcm2835_gpio_set(GPIO_TOF_9);
	delay(10);
	globalSensors[8] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[8]->setAddress(0x38);
	delay(10);
	puts("Sensor nine started at: 0x38");

	bcm2835_gpio_set(GPIO_TOF_10);
	delay(10);
	globalSensors[9] = new VL53L1X(VL53L1X::Medium,0x29);
	delay(10);
	globalSensors[9]->setAddress(0x39);
	delay(10);
	puts("Sensor ten started at: 0x39");


	for(int i=0; i<10; i++){
		globalSensors[i]->startContinuous(20);
		delay(10);
	}
	printf("started coninous");

	for(int j=0; j<1000; j++){
	//while(1){
		//for(int i=0; i<10; i++){
		//			measurement[i] = globalSensors[i]->readData(1);
		//			printf("%d:%5d ",i,measurement[i]);
		//}
				measurement[5] = globalSensors[5]->readData(1);
				printf("5:%5d ",measurement[5]);

		printf("\n");
		delay(20);
	}

	for(int i=0; i<10; i++){
		globalSensors[i]->disable();
		delay(10);
	}

	file.close();
}
