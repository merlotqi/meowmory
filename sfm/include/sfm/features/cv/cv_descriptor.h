#pragma once

#include <sfm/features/descriptor.h>

#include <opencv2/features2d.hpp>
#include <vector>

namespace meow::sfm {

class CvSiftDescriptor : public IFeatureDescriptor {
 public:
  CvSiftDescriptor();
  ~CvSiftDescriptor() override;

  std::vector<Descriptor> compute(const cv::Mat& image, const std::vector<KeyPoint>& keypoints) override;

 private:
  cv::Ptr<cv::SIFT> sift_;
};

cv::KeyPoint to_cv_keypoint(const KeyPoint& kp);
Descriptor from_cv_mat(const cv::Mat& desc_mat, int row, const KeyPoint& kp);

}  // namespace meow::sfm
