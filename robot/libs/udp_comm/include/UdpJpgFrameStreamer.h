#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <boost/circular_buffer.hpp>
using namespace cv;
using namespace std;
using boost::asio::ip::udp;

typedef boost::circular_buffer<Mat> frames_circular_buffer;

class UdpJpgFrameStreamer {
	private:
		int port;
		int dataSize;
		int bufferSize;
		boost::asio::io_service io_service;
     	udp::endpoint remote_endpoint;
		udp::socket* socket;
		vector<int> compression_params;
		frames_circular_buffer frames_buffer;
		
	public:
		UdpJpgFrameStreamer(int port=2024, int dataSize = 64000, int jpegCompressionLevel = 80, int bufferSize = 1);
		void waitForClient();
		void run();
		void runStream();
		void uploadFrame(Mat frame); 
		void pushFrame(Mat frame); 
};

