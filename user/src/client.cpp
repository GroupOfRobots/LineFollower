#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <chrono>
using namespace cv;
using namespace std;
using boost::asio::ip::udp;

double scaleFactor = 1;
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;

Mat src; Mat dst;
int chunkSize = 64000;
/// Function headers
int display_dst( int delay, char* window_name);

int main(int argc, char* argv[])
{
  try
  {
      std::cerr << "ok1" << std::endl;
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), argv[1], "2024");
    udp::endpoint receiver_endpoint = *resolver.resolve(query);
    udp::socket socket(io_service);
    socket.open(udp::v4());
    boost::array<char, 1> send_buf  = {{ 0 }};
    boost::array<char, 64000> recv_buf;
    udp::endpoint sender_endpoint;
    std::size_t bytesReceived =1;

    int counter = 0;
	  double total_comm_time = 0;
    double total_preproc_and_disp_time = 0;
    double total_bytes = 0;
	  int max_counter = 1000;

    while(bytesReceived != 0){ 
      auto start = chrono::steady_clock::now();
      socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);     
      bytesReceived= socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
      auto end = chrono::steady_clock::now();
      total_comm_time += chrono::duration_cast<chrono::microseconds>(end - start).count();
      total_bytes += bytesReceived;

      start = chrono::steady_clock::now();
      //std::cout.write(recv_buf.data(), bytesReceived);
      if(recv_buf.empty()){std::cerr << "Buf is empty, got " <<bytesReceived << "bytes"<< std::endl;}
      std:vector<char> data(recv_buf.begin(), recv_buf.end());
      dst=imdecode(data,1);
	    resize(dst, dst, Size(0,0), scaleFactor, scaleFactor, 1);
      display_dst(27,"Received");
      end = chrono::steady_clock::now();
      total_preproc_and_disp_time += chrono::duration_cast<chrono::microseconds>(end - start).count();

      ++counter;

      if(counter == max_counter){
		    cout << "Communication time in microseconds: "<<total_comm_time/max_counter<<" µs" << endl;
        cout << "Preproc and disp time in microseconds: "<<total_preproc_and_disp_time/max_counter<<" µs" << endl;
        cout << "Avg bytes received: "<<total_bytes/max_counter<<" bytes" << endl;
        counter = 0;
				total_comm_time = 0;
        total_preproc_and_disp_time = 0;
        total_bytes = 0;
      }

	
	}
	destroyAllWindows();
    

  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

 int display_dst( int delay, char* window_name )
  {
    imshow( window_name, dst );
    int c = waitKey ( delay );
    if( c >= 0 ) { return -1; }
    return 0;
  }
