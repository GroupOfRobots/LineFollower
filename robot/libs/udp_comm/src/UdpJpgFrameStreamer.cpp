#include "UdpJpgFrameStreamer.h"

UdpJpgFrameStreamer::UdpJpgFrameStreamer(int port, int dataSize, int jpegCompressionLevel){
	this->dataSize = dataSize;
	this->port = port;
	socket = new udp::socket(io_service, udp::endpoint(udp::v4(), port));
	compression_params.push_back(1);//CV_IMWRITE_JPEG_QUALITY
	compression_params.push_back(jpegCompressionLevel);
	ready_to_send = false;
}

void UdpJpgFrameStreamer::waitForClient(){
	try{
		boost::array<char, 1> recv_buf;
		boost::system::error_code error;
		socket->receive_from(boost::asio::buffer(recv_buf),remote_endpoint, 0, error);
		if (error && error != boost::asio::error::message_size)
       		throw boost::system::system_error(error);
	}
	catch (std::exception& e)
  	{
   		std::cerr << e.what() << std::endl;
  	}
}

void UdpJpgFrameStreamer::run(){
	std::thread streamer(&UdpJpgFrameStreamer::runStream, this);
	streamer.detach();
}

void UdpJpgFrameStreamer::runStream(){
	while(1){
		std::cout<<"no nie"<<endl;
		if(ready_to_send){
			uploadFrame();
			ready_to_send = false;
		}
	}
}

void UdpJpgFrameStreamer::pushFrame(Mat frame){
	frame_to_send = frame;
	ready_to_send = true;
}

void UdpJpgFrameStreamer::uploadFrame(){
	waitForClient();
	std::vector<uchar> buffer;
    imencode(".jpg", frame_to_send, buffer, compression_params);//czwarty parametr to kompresja obrazku
    //std::cerr << buffer.size()<<endl;
	boost::system::error_code ignored_error;
    string chunk=string(buffer.begin(),buffer.end());
	
	try{
	    socket->send_to(boost::asio::buffer(chunk),remote_endpoint, 0, ignored_error);
	}

	catch (std::exception& e)
  	{
   		std::cerr << e.what() << std::endl;
  	}
}

