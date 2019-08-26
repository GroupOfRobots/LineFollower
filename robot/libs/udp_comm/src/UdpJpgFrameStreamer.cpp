#include "UdpJpgFrameStreamer.h"

UdpJpgFrameStreamer::UdpJpgFrameStreamer(int port, int dataSize, int jpegCompressionLevel, int bufferSize){
	this->dataSize = dataSize;
	this->port = port;
	this->bufferSize = bufferSize;
	socket = new udp::socket(io_service, udp::endpoint(udp::v4(), port));
	compression_params.push_back(1);//CV_IMWRITE_JPEG_QUALITY
	compression_params.push_back(jpegCompressionLevel);
	frames_buffer.resize(bufferSize);
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
		if(!frames_buffer.empty() && (frames_buffer[0].empty() || frames_buffer[0].rows==-1 || frames_buffer[0].cols==-1)){
			Mat frame = frames_buffer[0];
			uploadFrame(frame);
			frames_buffer.pop_front();
		}
	}
}

void UdpJpgFrameStreamer::pushFrame(Mat frame){
	frames_buffer.push_back(frame);
}

void UdpJpgFrameStreamer::uploadFrame(Mat frame){
	std::vector<uchar> buffer;
    imencode(".jpg", frame, buffer, compression_params);//czwarty parametr to kompresja obrazku
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

