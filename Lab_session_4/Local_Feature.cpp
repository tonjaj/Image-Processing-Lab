#include "opencv2/opencv.hpp"

#if (defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2) //opencv version 2.4.x
#include "opencv2/nonfree/nonfree.hpp"
#else  //opencv version 3.x
//to support SIFT and sURF in version 3 compile from source :(
#endif

int main(int argc, char**argv){
#if (defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2)
	cv::initModule_nonfree();
#endif
	bool visualize_everything = true;
	cv::Mat model = cv::imread("../data/target.jpg");
	cv::Mat scene = cv::imread("../data/scene6.jpg");
	if (model.data == NULL || scene.data == NULL){
		std::cout << "Unable to load image..." << std::endl;
		std::exit(1);
	}
	std::cout << "Model Image loaded..." << std::endl;

	int max_keypoint = 1000;
#if (defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2)
	cv::Ptr<cv::Feature2D> detector = cv::Feature2D::create("SIFT");
	cv::Ptr<cv::Feature2D> descriptor = cv::Feature2D::create("SIFT");
#else
	cv::Ptr<cv::Feature2D> detector = cv::BRISK::create();
	cv::Ptr<cv::Feature2D> descriptor = cv::BRISK::create();
#endif

	std::vector<cv::KeyPoint> keypoints_model, keypoints_scene;

	//STEP 1: keypoint detection
	//model detection
	detector->detect(model, keypoints_model);
	std::cout << keypoints_model.size() << " Keypoints found on model" << std::endl;
	////scene detection
	detector->detect(scene, keypoints_scene);
	std::cout << keypoints_scene.size() << " Keypoints found on scene" << std::endl;

	if (visualize_everything){
		//show detected keypoints
		cv::Mat target_copy, scene_copy;
		cv::drawKeypoints(model, keypoints_model, target_copy,cv::Scalar::all(-1),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		cv::imshow("Model", target_copy);
		cv::drawKeypoints(scene, keypoints_scene, scene_copy, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		cv::namedWindow("Scene", cv::WINDOW_NORMAL);
		cv::imshow("Scene", scene_copy);
		std::cout << "Keypoint detected" << std::endl;
		cv::waitKey();
		cv::destroyAllWindows();
	}

	//STEP 2: Keypoint description
	//create descriptors
	cv::Mat descriptor_model, descriptor_scene;
	descriptor->compute(model, keypoints_model, descriptor_model);
	descriptor->compute(scene, keypoints_scene, descriptor_scene);
	std::cout << "Descriptor computed" << std::endl;

	//STEP 3: Descriptor MAtching using FLANN
	//create matcher

#if (defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH == 2)
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");
#else
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::makePtr<cv::FlannBasedMatcher>(new cv::flann::LshIndexParams(10, 20, 2));
#endif
	
	std::vector<cv::Mat> models_descriptors;
	models_descriptors.push_back(descriptor_model);
	//add model descriptor
	matcher->add(models_descriptors);

	//matching
	std::vector<std::vector<cv::DMatch>> matches;
	matcher->knnMatch(descriptor_scene, matches, 2);
	std::cout << matches.size() << " match founded" << std::endl;

	//ratio test to discard bad matches
	std::vector<cv::DMatch> filtered_match;
	std::vector<cv::DMatch> all_match;
	float ratio_th = 0.8;
	for (int i = 0; i < matches.size(); i++){
		std::vector<cv::DMatch> current_match = matches[i];
		if (current_match.size() == 2){
			all_match.push_back(current_match[0]);
			if ((current_match[0].distance / current_match[1].distance) < ratio_th){
				filtered_match.push_back(current_match[0]);
			}
		}
	}
	std::cout << filtered_match.size() << " good match after ratio test" << std::endl;

	if (visualize_everything){
		//draw matches
		cv::Mat result_pre_filter, result;
		cv::drawMatches(scene, keypoints_scene, model, keypoints_model, all_match, result_pre_filter, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		cv::drawMatches(scene, keypoints_scene, model, keypoints_model, filtered_match, result);
		cv::namedWindow("Not filtered", cv::WINDOW_NORMAL);
		cv::namedWindow("Filtered", cv::WINDOW_NORMAL);
		cv::imshow("Not filtered", result_pre_filter);
		cv::imshow("Filtered", result);
		cv::waitKey();
		cv::destroyAllWindows();
	}

	//STEP 4: Estimate Homograpy with RANSAC
	//extract pair of matching keypoints
	std::vector<cv::Point2d> model_points;
	std::vector<cv::Point2d> scene_points;
	for (int j = 0; j < filtered_match.size(); j++){
		cv::DMatch current = filtered_match[j];
		model_points.push_back(keypoints_model[current.trainIdx].pt);
		scene_points.push_back(keypoints_scene[current.queryIdx].pt);
	}

	//estimate homography using ransac
	cv::Mat homography = cv::findHomography(model_points, scene_points, CV_RANSAC);
	std::cout << "Homography estimated" << std::endl;
	cv::Mat bounding_box;
	scene.copyTo(bounding_box);

	//compute scene bounding box corner using the homography computed right now
	std::vector<cv::Point2f> obj_corners(4);
	obj_corners[0] = cv::Point(0, 0);
	obj_corners[1] = cv::Point(model.cols, 0);
	obj_corners[2] = cv::Point(model.cols, model.rows);
	obj_corners[3] = cv::Point(0, model.rows);
	std::vector<cv::Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, homography);
	//draw the bounding box
	cv::line(bounding_box, scene_corners[0], scene_corners[1], cv::Scalar(0, 255, 0), 6);
	cv::line(bounding_box, scene_corners[1], scene_corners[2], cv::Scalar(0, 255, 0), 6);
	cv::line(bounding_box, scene_corners[2], scene_corners[3], cv::Scalar(0, 255, 0), 6);
	cv::line(bounding_box, scene_corners[3], scene_corners[0], cv::Scalar(0, 255, 0), 6);

	cv::namedWindow("Bounding box", cv::WINDOW_NORMAL);
	cv::imshow("Bounding box", bounding_box);
	cv::waitKey();
	cv::destroyAllWindows();
}