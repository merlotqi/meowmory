#pragma once

#include <sfm/features/feature.h>

#include <vector>

namespace meow::sfm {

class IFeatureMatcher {
 public:
  virtual ~IFeatureMatcher() = default;
  virtual std::vector<FeatureMatch> match(const std::vector<Descriptor>& query,
                                          const std::vector<Descriptor>& train) = 0;
};

}  // namespace meow::sfm
