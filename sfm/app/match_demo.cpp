#include <sfm/features/cv/cv_detector.h>
#include <sfm/features/cv/cv_descriptor.h>
#include <sfm/features/cv/cv_matcher.h>
#include <sfm/features/sift/sift_pipeline.h>
#include <sfm/sfm.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: match_demo <image1> <image2>\n";
    return 1;
  }

  cv::Mat img1 = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  cv::Mat img2 = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
  if (img1.empty() || img2.empty()) {
    std::cerr << "Failed to load images\n";
    return 1;
  }

  meow::sfm::CvSiftDetector detector;
  meow::sfm::CvSiftDescriptor descriptor;
  meow::sfm::CvBFMatcher matcher(0.75F);
  meow::sfm::SiftPipeline pipeline(detector, descriptor, matcher);

  auto result = pipeline.process(img1, img2);

  std::cout << "Keypoints image 1: " << result.query_keypoints.size() << "\n"
            << "Keypoints image 2: " << result.train_keypoints.size() << "\n"
            << "Matches: " << result.matches.size() << "\n";

  cv::Mat img1_color, img2_color;
  cv::cvtColor(img1, img1_color, cv::COLOR_GRAY2BGR);
  cv::cvtColor(img2, img2_color, cv::COLOR_GRAY2BGR);

  std::vector<cv::DMatch> cv_matches;
  for (const auto& m : result.matches) {
    cv_matches.emplace_back(m.query_idx, m.train_idx, m.distance);
  }

  std::vector<cv::KeyPoint> cv_kp1, cv_kp2;
  for (const auto& kp : result.query_keypoints) {
    cv_kp1.emplace_back(cv::Point2f{(float)kp.pt.x, (float)kp.pt.y},
                        (float)kp.size);
  }
  for (const auto& kp : result.train_keypoints) {
    cv_kp2.emplace_back(cv::Point2f{(float)kp.pt.x, (float)kp.pt.y},
                        (float)kp.size);
  }

  cv::Mat match_img;
  cv::drawMatches(img1_color, cv_kp1, img2_color, cv_kp2, cv_matches,
                  match_img);
  cv::namedWindow("SIFT Matches", cv::WINDOW_NORMAL);
  cv::imshow("SIFT Matches", match_img);
  cv::waitKey(0);

  return 0;
}
