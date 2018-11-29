#include "opencv2/opencv.hpp"

int main(void) {

	cv::Mat image_1 = cv::imread("../data/fd1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat image_2 = cv::imread("../data/fd2.jpg", CV_LOAD_IMAGE_GRAYSCALE);


	//Check if the images are properly laoded
	if (image_1.data == NULL) {
		std::cout << "Unable to load image with file path: " << "../data/fd1.jpg" << std::endl;
		std::cout << "Ending execution..." << std::endl;
		return 0;
	}

	if (image_2.data == NULL) {
		std::cout << "Unable to load image with file path: " << "../data/fd2.jpg" << std::endl;
		std::cout << "Ending execution..." << std::endl;
		return 0;
	}

	//Print some image information
	std::cout << "Image " << "../data/fd1.jpg" << " succesfully loaded." << std::endl;
	std::cout << "Image " << "../data/fd2.jpg" << " succesfully loaded." << std::endl;

	cv::Mat image_3(image_1.size(), image_1.type());
	image_3	= image_1 - image_2;

	cv::imshow("image deviation", image_3);
}