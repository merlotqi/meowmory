#pragma once

#include <sfm/features/detector.h>

#include <opencv2/features2d.hpp>
#include <vector>

namespace meow::sfm {

class CvSiftDetector : public IFeatureDetector {
 public:
  CvSiftDetector();
  ~CvSiftDetector() override;

  std::vector<KeyPoint> detect(const cv::Mat& image) override;

 private:
  cv::Ptr<cv::SIFT> sift_;
};

KeyPoint from_cv_keypoint(const cv::KeyPoint& cv_kp, int octave);

}  // namespace meow::sfm
