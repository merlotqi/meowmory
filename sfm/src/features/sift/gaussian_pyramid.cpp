#include <opencv2/core/hal/interface.h>
#include <sfm/features/sift/gaussian_pyramid.h>

#include <cassert>
#include <cmath>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

namespace meow::sfm {

GaussianPyramid build_gaussian_pyramid(const cv::Mat& image, int n_octaves, int n_scales, double sigma0) {
  assert(!image.empty());
  assert(image.type() == CV_8UC1);
  assert(n_scales >= 1);
  assert(sigma0 > 0.0);

  if (n_octaves <= 0) {
    // Auto-compute: maximum number of octaves such that the smallest image is at least 8×8 pixels
    n_octaves = static_cast<int>(std::floor(std::log2(std::min(image.cols, image.rows))) - 3);
    if (n_octaves < 1) {
      n_octaves = 1;
    }
  }

  const double k = std::pow(2.0, 1.0 / n_scales);
  // We need n_scales + 3 blurred images per octave to produce
  // n_scales + 2 DoG images, which gives us n_scales usable DoG
  // levels for extrema detection (3×3×3 neighborhood).
  const int n_images = n_scales + 3;

  GaussianPyramid pyramid;
  pyramid.sigma0 = sigma0;
  pyramid.n_scales = n_scales;
  pyramid.octaves.resize(n_octaves);

  // Convert to float for precision in Gaussian convolution
  cv::Mat current;
  image.convertTo(current, CV_32F, 1.0 / 255.0);

  for (int o = 0; o < n_octaves; ++o) {
    auto& octave = pyramid.octaves[o];
    octave.scales.resize(n_images);

    // Pre-blur the first image of this octave if octave > 0
    if (o == 0) {
      cv::resize(current, current, cv::Size(current.cols * 2, current.rows * 2), 0, 0, cv::INTER_NEAREST);
      // Blur to sigma0 (assume input has nominal blur of 0.5)
      double sigma_prev = 0.5;
      double sigma_blur = std::sqrt(sigma0 * sigma0 - sigma_prev * sigma_prev);
      if (sigma_blur > 0.0) {
        cv::GaussianBlur(current, current, cv::Size(0, 0), sigma_blur, sigma_blur);
      }
    }

    for (int s = 0; s < n_images; ++s) {
      if (s == 0 && o == 0) {
        // First image: already blurred to sigma0
        octave.scales[s] = current.clone();
      } else if (s == 0) {
        // First image of subsequent octave: downsample from previous octave
        cv::resize(current, current, cv::Size(current.cols / 2, current.rows / 2), 0, 0, cv::INTER_NEAREST);
        octave.scales[s] = current.clone();
      } else {
        // Blur the previous scale
        double sig_prev = sigma0 * std::pow(k, s - 1);
        double sig_curr = sigma0 * std::pow(k, s);
        double sig_diff = std::sqrt(sig_curr * sig_curr - sig_prev * sig_prev);
        cv::GaussianBlur(octave.scales[s - 1], octave.scales[s], cv::Size(0, 0), sig_diff, sig_diff);
      }
    }

    // Prepare current for next octave: use the 3rd-from-last scale
    // (scale at 2*sigma0 in this octave, becomes sigma0 when downsampled)
    int src_idx = n_images - 3;  // = n_scales
    current = octave.scales[src_idx].clone();
  }

  return pyramid;
}

}  // namespace meow::sfm
