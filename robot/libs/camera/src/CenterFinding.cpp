#include "CenterFinding.h"

CenterFinding::CenterFinding(){}

CenterFinding::CenterFinding(Mat frame){
	sourceFrame = frame;
	outputFrame = frame;
}

void CenterFinding::cutImage(){
	Rect r(0, 0, outputFrame.cols, 2*(outputFrame.rows)/3);
	outputFrame = outputFrame(r);
}

void CenterFinding::toGrayScale(){
	cvtColor(outputFrame, outputFrame, cv::COLOR_RGB2GRAY);
}
void CenterFinding::useBlur(){
	medianBlur (outputFrame, outputFrame, max_kernel_length);
}

void CenterFinding::erodeFrame(Mat element){
	erode(outputFrame, outputFrame, element );
}

void CenterFinding::dilateFrame(Mat element){
	dilate(outputFrame, outputFrame, element);
}

void CenterFinding::thresholdFrame(){
	threshold(outputFrame, outputFrame, 127, 255, THRESH_BINARY_INV);
}

vector<Point> CenterFinding::findCenters(){
	vector<Point> centers;
	int spaceWidth = outputFrame.rows/8;
	//cout<<"space: "<<spaceWidth<<endl;
	int sum = 0;
	int center = 0;
	int whitePixelCounter = 0;

	for(int i = spaceWidth; i < outputFrame.rows; i+=spaceWidth)
	{
		for(int j = 0; j < outputFrame.cols; j++)
		{
			if(int(outputFrame.at<uchar>(i,j)) == 255)
			{
				sum+=j;
				whitePixelCounter++;
			}
			//cout<<"row: "<<i<<" column: "<<j<<endl;
		}

		if(whitePixelCounter != 0)
		{
			center = sum/whitePixelCounter;
			centers.push_back(Point(center,i));
		}
	
		sum=0;
		whitePixelCounter=0;
	}

	return centers;
}

void CenterFinding::setSourceFrame(Mat frame){
	sourceFrame = frame;
	outputFrame = frame;
}

Mat CenterFinding::getSourceFrame(){
	return sourceFrame;
}

Mat CenterFinding::getOutputFrame(){
	return outputFrame;
}

vector<Point> CenterFinding::findLineCenters(){
	cutImage();

	toGrayScale();

	useBlur();

	Mat element = getStructuringElement( MORPH_RECT,Size( 2*max_kernel_size + 1, 2*max_kernel_size+1 ),Point( max_kernel_size, max_kernel_size ));

	erodeFrame(element);

	dilateFrame(element);

	thresholdFrame();

	return findCenters();
}

Mat CenterFinding::drawPoints(vector<Point> centers){
	Mat frame = sourceFrame.clone();

	//rysowanie wykrytych środków ciężkosci linii
	Scalar colorPoint(0, 0, 255);
	Scalar colorLine(0, 255, 0);

	//rysowanie linii i jej środka ciężkosci
	for(int i=0;i<centers.size();i++)
	{
		line(frame,Point(0,centers[i].y),Point(frame.cols,centers[i].y),colorLine);
    		circle(frame, centers[i], 5, colorPoint, -1, 8);
    	}

	return frame;
}
		
