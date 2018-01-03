// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void logic1();
void logic2();
Mat erase_dot1(Mat oldmat);
Mat erase_dot2(Mat oldmat);
bool nearWhitePixel(Mat oldmat, Mat newmat, int r, int c, int findline);

int main(int argc, char** argv) {
	cout << "!!!Hello World!!!" << endl; 
	
	logic1();
	
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}

void logic1() {
	String imageName("D:\\magichat\\findline.png"); // by default
	
	Mat image;
	image = imread(imageName, IMREAD_COLOR); // Read the file
	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		//return -1;
	}

	Mat gray, edges, edges_cv8u, draw;
	cvtColor(image, gray, CV_BGR2GRAY);

	Canny(gray, edges, 100, 150, 3);
//	Canny(gray, edges, 50, 150, 3);
	edges.convertTo(edges_cv8u, CV_8U);
	//Mat.at(i, j) == if (Mat.type == CV_8U)-- > Mat.at<unsigned char>(i, j);
	//Mat.at(i, j) == if (Mat.type == CV_32F)-- > Mat.at<float>(i, j);

	draw = erase_dot2(edges_cv8u);

	
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", draw);                // Show our image inside it.

}

Mat erase_dot1(Mat oldmat) {
	Mat newmat = oldmat.clone();
	int min_r = 0, max_r = oldmat.rows, min_c = 0, max_c = oldmat.cols;
	int findline = 0;
	for (int r = min_r; r < max_r; r++) {
		for (int c = min_c; c < max_c; c++) {
			if (oldmat.at<unsigned char>(r, c) != 0) {
				findline = 0;
				for (int tmp_r = r - 2; tmp_r < r + 3; tmp_r++) {
					for (int tmp_c = c - 2; tmp_c < c + 3; tmp_c++) {
						if (min_c <= tmp_c && tmp_c < max_c && min_r <= tmp_r && tmp_r < max_r) {
							if (oldmat.at<unsigned char>(tmp_r, tmp_c) != 0) {
								findline++;
							}
						}
					}
				}
				if (5 < findline && findline < 10) {
					//cout << draw.at<unsigned char>(r, c) << endl;
					newmat.at<unsigned char>(r, c) = 0;
				}
			}
		}
	}

	return newmat;
}


Mat erase_dot2(Mat oldmat) {
	Mat newmat = oldmat.clone();
	int min_r = 0, max_r = oldmat.rows, min_c = 0, max_c = oldmat.cols;
	int findline = 0;
	for (int r = min_r; r < max_r; r++) {
		for (int c = min_c; c < max_c; c++) {
			findline = 0;
			nearWhitePixel(oldmat, newmat, r, c, findline);
		}
	}

	return newmat;
}

bool nearWhitePixel(Mat oldmat, Mat newmat, int r, int c, int findline) {
	int min_r = 0, max_r = oldmat.rows, min_c = 0, max_c = oldmat.cols;
	if (min_c <= c && c < max_c && min_r <= r && r < max_r && oldmat.at<unsigned char>(r, c) != 0) {
		findline++;
		for (int tmp_r = r - 5; tmp_r < r + 6; tmp_r++) {
			for (int tmp_c = c - 5; tmp_c < c + 6; tmp_c++) {
				if (min_c <= tmp_c && tmp_c < max_c && min_r <= tmp_r && tmp_r < max_r) {
					bool erase_flag = nearWhitePixel(oldmat, newmat, tmp_r, tmp_c, findline);
					if (erase_flag) {
						newmat.at<unsigned char>(tmp_r, tmp_c) = 0;
						return true;
					}
					else {
						return false;
						//continuously searching
					}
				}
			}
		}
	}
	else {
		if (0 < findline && findline < 15) {
			return true;
		}
		else {
			return false;
		}
	}
}

void logic2() {
	String filename("D:\\magichat\\sample.mp4"); // by default
	VideoCapture cap(filename);
	if (!cap.isOpened()) {
		cout << "!!!Not Open!!!" << endl;
	}

	Mat edges;
	namedWindow("edges", 1);
	Mat curframe, preframe;
	for (;;) {
		preframe = curframe.clone();
		cap >> curframe; // get a new frame from camera
						 //imshow("edges", curframe);
		edges = curframe - preframe;

		int min_y = 99999, max_y = 0, min_x = 99999, max_x = 0;
		for (int y = 0; y < edges.rows; y++) {
			for (int x = 0; x < edges.cols; x++) {
				//edges.at<Vec3b>(y,x)[1,2,3] RGB
				if (edges.at<uchar>(y, x) != 0) {
					//Not Black
					if (y < min_y) { min_y = y; }
					if (y > max_y) { max_y = y; }
					if (x < min_x) { min_x = x; }
					if (x > max_x) { max_x = x; }
				}
			}
		}
		if (min_x == 0) { min_x = 1; }
		if (min_y == 0) { min_y = 1; }

		int thickness = 2;
		int lineType = 8;
		line(edges, Point(max_y, max_x), Point(min_y, max_x), Scalar(255, 0, 0), thickness, lineType);
		line(edges, Point(max_y, min_x), Point(min_y, min_x), Scalar(255, 0, 0), thickness, lineType);
		line(edges, Point(max_y, max_x), Point(max_y, min_x), Scalar(255, 0, 0), thickness, lineType);
		line(edges, Point(min_y, max_x), Point(min_y, min_x), Scalar(255, 0, 0), thickness, lineType);

		imshow("edges", edges);
		if (waitKey(30) >= 0) break;
	}
}