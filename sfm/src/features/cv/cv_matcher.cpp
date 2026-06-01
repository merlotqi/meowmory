#include <sfm/features/cv/cv_matcher.h>

#include <cassert>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

namespace meow::sfm {

CvBFMatcher::CvBFMatcher(float ratio) : ratio_(ratio) { assert(ratio > 0.0F && ratio < 1.0F); }

std::vector<FeatureMatch> CvBFMatcher::match(const std::vector<Descriptor>& query,
                                             const std::vector<Descriptor>& train) {
  if (query.empty() || train.empty()) return {};

  auto to_cv_mat = [](const std::vector<Descriptor>& descs) -> cv::Mat {
    cv::Mat mat(static_cast<int>(descs.size()), 128, CV_32F);
    for (size_t i = 0; i < descs.size(); ++i) {
      float* row = mat.ptr<float>(static_cast<int>(i));
      for (int j = 0; j < 128; ++j) {
        row[j] = static_cast<float>(descs[i].data[j]);
      }
    }
    return mat;
  };

  cv::Mat query_mat = to_cv_mat(query);
  cv::Mat train_mat = to_cv_mat(train);

  cv::BFMatcher bf(cv::NORM_L2);
  std::vector<std::vector<cv::DMatch>> knn_matches;
  bf.knnMatch(query_mat, train_mat, knn_matches, 2);

  std::vector<FeatureMatch> result;
  for (const auto& knn : knn_matches) {
    if (knn.size() < 2) continue;
    if (knn[0].distance < ratio_ * knn[1].distance) {
      FeatureMatch fm;
      fm.query_idx = knn[0].queryIdx;
      fm.train_idx = knn[0].trainIdx;
      fm.distance = knn[0].distance;
      result.push_back(fm);
    }
  }
  return result;
}

}  // namespace meow::sfm
