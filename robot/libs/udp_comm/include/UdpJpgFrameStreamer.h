#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
using boost::asio::ip::udp;

class UdpJpgFrameStreamer {
	private:
		int port;
		int dataSize;
		boost::asio::io_service io_service;
     	udp::endpoint remote_endpoint;
		udp::socket* socket;
		vector<int> compression_params;
		
	public:
		UdpJpgFrameStreamer(int port=2024, int dataSize = 64000, int jpegCompressionLevel = 80);
		void waitForClient();
		void pushFrame(Mat frame); 
};

