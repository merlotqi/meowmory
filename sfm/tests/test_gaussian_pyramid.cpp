// sfm/tests/test_gaussian_pyramid.cpp
#include <sfm/features/sift/gaussian_pyramid.h>

#include <catch2/catch_test_macros.hpp>
#include <opencv2/core.hpp>

TEST_CASE("GaussianPyramid builds correct number of octaves and scales", "[pyramid]") {
  cv::Mat img(128, 128, CV_8UC1, cv::Scalar(128));

  auto pyr = meow::sfm::build_gaussian_pyramid(img, 4, 3, 1.6);

  // 4 octaves requested
  REQUIRE(pyr.octaves.size() == 4);

  // n_scales + 3 images per octave
  for (const auto& oct : pyr.octaves) {
    REQUIRE(oct.scales.size() == 6);  // 3 + 3
  }

  // Verify sigma values
  CHECK(pyr.sigma0 == 1.6);
  CHECK(pyr.n_scales == 3);
}

TEST_CASE("GaussianPyramid images decrease in size across octaves", "[pyramid]") {
  cv::Mat img(256, 256, CV_8UC1, cv::Scalar(128));

  auto pyr = meow::sfm::build_gaussian_pyramid(img, 3, 3, 1.6);

  // Octave 0 is doubled, so 512×512; then halves each octave
  CHECK(pyr.octaves[0].scales[0].cols > img.cols);  // doubled
  CHECK(pyr.octaves[1].scales[0].cols < pyr.octaves[0].scales[0].cols);
  CHECK(pyr.octaves[2].scales[0].cols < pyr.octaves[1].scales[0].cols);
}

TEST_CASE("GaussianPyramid with auto octave count", "[pyramid]") {
  cv::Mat img(64, 64, CV_8UC1, cv::Scalar(128));

  // auto-compute: floor(log2(64)) - 3 = 6 - 3 = 3
  auto pyr = meow::sfm::build_gaussian_pyramid(img);

  CHECK(pyr.octaves.size() >= 1);
}
