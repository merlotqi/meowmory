# SfM Module Design: Phase 1 — Custom SIFT Pipeline

**Date**: 2026-06-01
**Status**: In Review
**Phase**: 1 (From-scratch technical accumulation)

## Overview

Build the SfM module's feature extraction layer from scratch, using a spiral development
approach. The goal is to learn and validate the SIFT algorithm deeply, using OpenCV's
SIFT as a reference for quantitative comparison at each step.

Use OpenCV native types (`cv::Mat`, `cv::Point2d`, etc.) directly throughout the module.
No Eigen3, no image wrapper — keep the dependency surface minimal. The core SIFT algorithm
(DoG detector + gradient histogram descriptor) is implemented from scratch on top of
OpenCV primitives.

## Development Approach: Spiral (Plan C)

Three concentric spirals, each producing a runnable end-to-end system:

### Spiral 1: OpenCV Full Pipeline
Wrap OpenCV SIFT + BFMatcher behind our interfaces. Deliver a working demo that reads
two images, detects keypoints, extracts descriptors, matches them, and visualizes the
result. This establishes the interface contracts and the demo harness.

### Spiral 2: Replace Detector
Implement from scratch and swap in:
- Gaussian pyramid (octave construction + downsampling)
- Difference of Gaussians (adjacent scale subtraction)
- Keypoint localization (extrema detection, subpixel refinement, edge suppression)

Compare against OpenCV SIFT: repeatability rate and keypoint position deviation.

### Spiral 3: Replace Descriptor
Implement from scratch and swap in:
- Dominant orientation assignment (gradient magnitude + orientation histogram)
- Descriptor construction (16×16 patch → 4×4 grid → 8-bin orientation histograms → 128D)
- Descriptor normalization (L2 → clamp 0.2 → L2)

Compare against OpenCV SIFT: descriptor distance distribution and matching score.

## Module Structure

```
sfm/
├── CMakeLists.txt
├── include/sfm/
│   ├── features/
│   │   ├── feature.h              # KeyPoint, Descriptor, FeatureMatch
│   │   ├── detector.h             # IFeatureDetector interface
│   │   ├── descriptor.h           # IFeatureDescriptor interface
│   │   ├── matcher.h              # IFeatureMatcher interface
│   │   ├── cv/                    # Spiral 1: OpenCV wrappers
│   │   │   ├── cv_detector.h      # cv::SIFT → IFeatureDetector
│   │   │   ├── cv_descriptor.h    # cv::SIFT → IFeatureDescriptor
│   │   │   └── cv_matcher.h       # cv::BFMatcher + ratio test → IFeatureMatcher
│   │   └── sift/
│   │       ├── sift_pipeline.h    # Wires detector+descriptor+matcher together
│   │       ├── sift_detector.h    # Custom DoG + keypoint localization (Spiral 2)
│   │       └── sift_descriptor.h  # Custom gradient histogram descriptor (Spiral 3)
│   └── sfm.h                      # Top-level include
├── src/
│   ├── CMakeLists.txt
│   └── features/
│       ├── cv/
│       │   ├── cv_detector.cpp
│       │   ├── cv_descriptor.cpp
│       │   └── cv_matcher.cpp
│       └── sift/
│           ├── sift_pipeline.cpp
│           ├── sift_detector.cpp
│           └── sift_descriptor.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test_detector.cpp
│   ├── test_descriptor.cpp
│   ├── test_matcher.cpp
│   └── test_data/
│       └── cat_*.jpg
└── app/
    └── match_demo.cpp
```

## Core Data Structures

All types use OpenCV primitives directly. No custom aliases, no wrappers.

```cpp
// sfm/features/feature.h
#include <opencv2/core.hpp>
#include <array>

struct KeyPoint {
    cv::Point2d pt;     // subpixel position
    double size;        // scale
    double angle;       // dominant orientation (radians)
    double response;    // DoG response strength
    int octave;         // pyramid octave index
};

struct Descriptor {
    std::array<uint8_t, 128> data; // 128D gradient histogram
    KeyPoint keypoint;             // attached keypoint (copy, not pointer)
};

struct FeatureMatch {
    int query_idx;      // index into query descriptors
    int train_idx;      // index into train descriptors
    double distance;    // L2 distance
};
```

## Interfaces

Operate on `cv::Mat` directly — no image wrapper.

```cpp
// sfm/features/detector.h
class IFeatureDetector {
public:
    virtual ~IFeatureDetector() = default;
    virtual std::vector<KeyPoint> detect(const cv::Mat& image) = 0;
};

// sfm/features/descriptor.h
class IFeatureDescriptor {
public:
    virtual ~IFeatureDescriptor() = default;
    virtual std::vector<Descriptor> compute(
        const cv::Mat& image,
        const std::vector<KeyPoint>& keypoints) = 0;
};

// sfm/features/matcher.h
class IFeatureMatcher {
public:
    virtual ~IFeatureMatcher() = default;
    virtual std::vector<FeatureMatch> match(
        const std::vector<Descriptor>& query,
        const std::vector<Descriptor>& train) = 0;
};
```

## Build System

- **CMake** project in `sfm/`, added via `add_subdirectory(sfm)` from root
- Dependencies: `OpenCV` only (`core`, `features2d`, `imgcodecs`, `imgproc`, `highgui`)
- No Eigen3 dependency during Phase 1 feature extraction
- Test framework: `Catch2` via `FetchContent`
- Demo binary: `match_demo` in `sfm/app/`

## Verification Strategy

| Layer | Content | Example |
|-------|---------|---------|
| Unit test | Pure logic of each component | DoG response vs hand-computed expected value |
| Benchmark test | Numerical comparison with OpenCV SIFT | Keypoint position deviation, descriptor distance distribution |
| Integration test | End-to-end matching visualization | match_demo outputs matching line image |

Benchmark tests do not enforce strict pass/fail thresholds. Instead they output
statistical distributions (mean / median / max deviation) for human review.
