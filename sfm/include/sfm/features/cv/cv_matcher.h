#pragma once

#include <sfm/features/matcher.h>

namespace meow::sfm {

class CvBFMatcher : public IFeatureMatcher {
 public:
  explicit CvBFMatcher(float ratio = 0.75F);

  std::vector<FeatureMatch> match(const std::vector<Descriptor>& query, const std::vector<Descriptor>& train) override;

 private:
  float ratio_;
};

}  // namespace meow::sfm
