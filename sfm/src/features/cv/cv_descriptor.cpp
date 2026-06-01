#include <sfm/features/cv/cv_descriptor.h>

#include <cassert>

namespace meow::sfm {

cv::KeyPoint to_cv_keypoint(const KeyPoint& kp) {
  cv::KeyPoint cv_kp;
  cv_kp.pt =
      cv::Point2f{static_cast<float>(kp.pt.x), static_cast<float>(kp.pt.y)};
  cv_kp.size = static_cast<float>(kp.size);
  cv_kp.angle = static_cast<float>(kp.angle);
  cv_kp.response = static_cast<float>(kp.response);
  cv_kp.octave = kp.octave;
  return cv_kp;
}

Descriptor from_cv_mat(const cv::Mat& desc_mat, int row, const KeyPoint& kp) {
  assert(desc_mat.type() == CV_32F);
  assert(desc_mat.cols == 128);
  assert(row >= 0 && row < desc_mat.rows);

  Descriptor desc;
  desc.keypoint = kp;

  const float* row_ptr = desc_mat.ptr<float>(row);
  for (int i = 0; i < 128; ++i) {
    float val = row_ptr[i];
    if (val < 0.0F) val = 0.0F;
    if (val > 255.0F) val = 255.0F;
    desc.data[i] = static_cast<uint8_t>(val);
  }
  return desc;
}

CvSiftDescriptor::CvSiftDescriptor() : sift_(cv::SIFT::create()) {
  assert(sift_ != nullptr);
}

CvSiftDescriptor::~CvSiftDescriptor() = default;

std::vector<Descriptor> CvSiftDescriptor::compute(
    const cv::Mat& image, const std::vector<KeyPoint>& keypoints) {
  assert(!image.empty());

  std::vector<cv::KeyPoint> cv_keypoints;
  cv_keypoints.reserve(keypoints.size());
  for (const auto& kp : keypoints) {
    cv_keypoints.push_back(to_cv_keypoint(kp));
  }

  cv::Mat desc_mat;
  sift_->compute(image, cv_keypoints, desc_mat);

  std::vector<Descriptor> result;
  result.reserve(desc_mat.rows);
  for (int i = 0; i < desc_mat.rows; ++i) {
    result.push_back(from_cv_mat(desc_mat, i, keypoints[i]));
  }
  return result;
}

}  // namespace meow::sfm
