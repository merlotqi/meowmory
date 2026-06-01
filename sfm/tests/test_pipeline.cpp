#include <sfm/features/cv/cv_descriptor.h>
#include <sfm/features/cv/cv_detector.h>
#include <sfm/features/cv/cv_matcher.h>
#include <sfm/features/sift/sift_pipeline.h>
#include <sfm/sfm.h>

#include <catch2/catch_test_macros.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace {

cv::Mat make_test_image() {
  cv::Mat img(256, 256, CV_8UC1, cv::Scalar(128));
  cv::rectangle(img, cv::Rect(64, 64, 128, 128), cv::Scalar(255), cv::FILLED);
  cv::GaussianBlur(img, img, cv::Size(3, 3), 1.0);
  return img;
}

}  // namespace

TEST_CASE("CvSiftDetector produces keypoints on structured image", "[detector]") {
  meow::sfm::CvSiftDetector detector;
  auto img = make_test_image();

  auto kps = detector.detect(img);

  CHECK(kps.size() > 0);
  for (const auto& kp : kps) {
    CHECK(kp.pt.x >= 0.0);
    CHECK(kp.pt.y >= 0.0);
    CHECK(kp.size > 0.0);
    CHECK(kp.response > 0.0);
    CHECK(kp.octave >= 0);
  }
}

TEST_CASE("CvSiftDescriptor produces 128D descriptors per keypoint", "[descriptor]") {
  meow::sfm::CvSiftDetector detector;
  meow::sfm::CvSiftDescriptor descriptor;
  auto img = make_test_image();

  auto kps = detector.detect(img);
  REQUIRE(!kps.empty());

  auto descs = descriptor.compute(img, kps);
  REQUIRE(descs.size() == kps.size());

  for (const auto& d : descs) {
    for (uint8_t v : d.data) {
      CHECK(v <= 255);
    }
    break;
  }
}

TEST_CASE("CvBFMatcher with ratio test on identical images", "[matcher]") {
  meow::sfm::CvSiftDetector detector;
  meow::sfm::CvSiftDescriptor descriptor;
  meow::sfm::CvBFMatcher matcher(0.75F);
  auto img = make_test_image();

  auto kp1 = detector.detect(img);
  auto kp2 = detector.detect(img);
  auto desc1 = descriptor.compute(img, kp1);
  auto desc2 = descriptor.compute(img, kp2);

  auto matches = matcher.match(desc1, desc2);

  CHECK(matches.size() > 0);
  for (const auto& m : matches) {
    CHECK(m.query_idx >= 0);
    CHECK(m.train_idx >= 0);
    CHECK(m.distance >= 0.0);
  }
}

TEST_CASE("SiftPipeline end-to-end produces consistent output", "[pipeline]") {
  meow::sfm::CvSiftDetector detector;
  meow::sfm::CvSiftDescriptor descriptor;
  meow::sfm::CvBFMatcher matcher(0.75F);
  meow::sfm::SiftPipeline pipeline(detector, descriptor, matcher);

  auto img1 = make_test_image();

  cv::Mat img2(256, 256, CV_8UC1, cv::Scalar(128));
  cv::rectangle(img2, cv::Rect(72, 72, 128, 128), cv::Scalar(255), cv::FILLED);
  cv::GaussianBlur(img2, img2, cv::Size(3, 3), 1.0);

  auto result = pipeline.process(img1, img2);

  CHECK(result.query_keypoints.size() > 0);
  CHECK(result.train_keypoints.size() > 0);
  CHECK(result.query_descriptors.size() == result.query_keypoints.size());
  CHECK(result.train_descriptors.size() == result.train_keypoints.size());
}
