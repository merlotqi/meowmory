#pragma once

#include <array>
#include <cstdint>
#include <opencv2/core.hpp>

/**
 * @namespace meow::sfm
 * @brief Structure from Motion module namespace
 *
 * Provides fundamental data structures for recovering 3D structure and camera motion
 * from multi-view image sequences. These structures are lightweight wrappers around
 * OpenCV native types, designed for independent use in the C++ Kernel with minimal
 * dependency on OpenCV runtime types.
 */
namespace meow::sfm {

/**
 * @struct KeyPoint
 * @brief Image feature point
 *
 * Represents a local region of interest in an image, used for matching and tracking
 * across multiple views. Similar to cv::KeyPoint but with a streamlined interface.
 *
 * A keypoint is characterized by its subpixel location, scale (size), orientation,
 * and quality (response). These properties enable scale-invariant and rotation-invariant
 * matching between different views of the same 3D scene point.
 */
struct KeyPoint {
  cv::Point2d pt;         ///< Subpixel coordinates (x, y) in image space
  double size = 0.0;      ///< Diameter of the meaningful keypoint neighborhood (scale)
  double angle = 0.0;     ///< Dominant orientation in radians [0, 2π)
  double response = 0.0;  ///< Strength/confidence of the keypoint (e.g., DoG response)
  int octave = 0;         ///< Pyramid octave index (0 = original resolution, higher = downsampled)
};

/**
 * @struct Descriptor
 * @brief Feature descriptor with its associated keypoint
 *
 * A descriptor is a numerical fingerprint that encodes the local image appearance
 * around a keypoint. Descriptors from different images can be compared (e.g., using
 * L2 distance or cosine similarity) to establish correspondences between views.
 *
 * The default descriptor type is a 128-dimensional vector of uint8_t, which is the
 * standard format for SIFT and similar handcrafted descriptors. Each dimension
 * represents a histogram bin of gradient orientations within a spatial subregion.
 */
struct Descriptor {
  std::array<uint8_t, 128> data{};  ///< 128-byte descriptor vector (SIFT-compatible format)
  KeyPoint keypoint;                ///< The keypoint from which this descriptor was computed
};

/**
 * @struct FeatureMatch
 * @brief Correspondence between two descriptors from different images
 *
 * Represents a putative correspondence between a feature in the query image
 * and a feature in the train image. The match quality is quantified by the
 * distance between their descriptor vectors.
 *
 * In practice, these matches are filtered by geometric verification (e.g., using
 * RANSAC with epipolar constraints) to reject outliers.
 */
struct FeatureMatch {
  int query_idx = -1;     ///< Index of the descriptor in the query image's descriptor list
  int train_idx = -1;     ///< Index of the descriptor in the train image's descriptor list
  double distance = 0.0;  ///< Distance metric between descriptors (typically L2)
};

}  // namespace meow::sfm
