#include <sfm/features/cv/cv_detector.h>

#include <cassert>

namespace meow::sfm {

KeyPoint from_cv_keypoint(const cv::KeyPoint& cv_kp, int octave) {
  KeyPoint kp;
  kp.pt = cv::Point2d{cv_kp.pt.x, cv_kp.pt.y};
  kp.size = cv_kp.size;
  kp.angle = cv_kp.angle;
  kp.response = cv_kp.response;
  kp.octave = octave;
  return kp;
}

CvSiftDetector::CvSiftDetector() : sift_(cv::SIFT::create()) { assert(sift_ != nullptr); }

CvSiftDetector::~CvSiftDetector() = default;

std::vector<KeyPoint> CvSiftDetector::detect(const cv::Mat& image) {
  assert(!image.empty());

  std::vector<cv::KeyPoint> cv_keypoints;
  sift_->detect(image, cv_keypoints);

  std::vector<KeyPoint> result;
  result.reserve(cv_keypoints.size());
  for (const auto& kp : cv_keypoints) {
    result.push_back(from_cv_keypoint(kp, kp.octave));
  }
  return result;
}

}  // namespace meow::sfm
