#include <sfm/features/sift/sift_pipeline.h>

#include <sfm/features/descriptor.h>
#include <sfm/features/detector.h>
#include <sfm/features/matcher.h>
#include <cassert>

namespace meow::sfm {

SiftPipeline::SiftPipeline(IFeatureDetector& detector,
                           IFeatureDescriptor& descriptor,
                           IFeatureMatcher& matcher)
    : detector_(detector), descriptor_(descriptor), matcher_(matcher) {}

MatchResult SiftPipeline::process(const cv::Mat& image1,
                                  const cv::Mat& image2) {
  assert(!image1.empty() && !image2.empty());

  MatchResult result;

  result.query_keypoints = detector_.detect(image1);
  result.train_keypoints = detector_.detect(image2);

  result.query_descriptors =
      descriptor_.compute(image1, result.query_keypoints);
  result.train_descriptors =
      descriptor_.compute(image2, result.train_keypoints);

  result.matches =
      matcher_.match(result.query_descriptors, result.train_descriptors);

  return result;
}

}  // namespace meow::sfm
