#pragma once

#include <sfm/features/feature.h>

#include <opencv2/core/mat.hpp>
#include <vector>

namespace meow::sfm {

class IFeatureDetector;
class IFeatureDescriptor;
class IFeatureMatcher;

struct MatchResult {
  std::vector<KeyPoint> query_keypoints;
  std::vector<KeyPoint> train_keypoints;
  std::vector<Descriptor> query_descriptors;
  std::vector<Descriptor> train_descriptors;
  std::vector<FeatureMatch> matches;
};

class SiftPipeline {
 public:
  SiftPipeline(IFeatureDetector& detector, IFeatureDescriptor& descriptor,
               IFeatureMatcher& matcher);

  MatchResult process(const cv::Mat& image1, const cv::Mat& image2);

 private:
  IFeatureDetector& detector_;
  IFeatureDescriptor& descriptor_;
  IFeatureMatcher& matcher_;
};

}  // namespace meow::sfm
