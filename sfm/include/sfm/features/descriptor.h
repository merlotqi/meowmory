#pragma once

#include <sfm/features/feature.h>

#include <opencv2/core/mat.hpp>
#include <vector>

namespace meow::sfm {

class IFeatureDescriptor {
 public:
  virtual ~IFeatureDescriptor() = default;
  virtual std::vector<Descriptor> compute(const cv::Mat& image, const std::vector<KeyPoint>& keypoints) = 0;
};

}  // namespace meow::sfm
