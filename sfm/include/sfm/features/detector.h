#pragma once

#include <sfm/features/feature.h>

#include <opencv2/core/mat.hpp>
#include <vector>

namespace meow::sfm {

class IFeatureDetector {
 public:
  virtual ~IFeatureDetector() = default;
  virtual std::vector<KeyPoint> detect(const cv::Mat& image) = 0;
};

}  // namespace meow::sfm
