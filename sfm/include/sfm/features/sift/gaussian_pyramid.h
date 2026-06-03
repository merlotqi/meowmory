#pragma once

#include <opencv2/core/mat.hpp>
#include <vector>

namespace meow::sfm {

struct PyramidOctave {
  std::vector<cv::Mat> scales;  // blurred images at increasing σ
};

struct GaussianPyramid {
  std::vector<PyramidOctave> octaves;
  double sigma0;  // base sigma
  int n_scales;   // intervals per octave
};

/// Build a Gaussian scale-space pyramid.
/// @param image  grayscale input, CV_8UC1
/// @param n_octaves  number of octaves (auto-computed if 0)
/// @param n_scales   intervals per octave (default 3)
/// @param sigma0     initial sigma (default 1.6)
GaussianPyramid build_gaussian_pyramid(const cv::Mat& image, int n_octaves = 0, int n_scales = 3, double sigma0 = 1.6);

}  // namespace meow::sfm
