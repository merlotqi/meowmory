# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Meowmory** — (virtual cat-raising): an iOS/iPadOS app that reconstructs a 3DGS (3D Gaussian Splatting) model of the user's cat from video input, then drives it with AI for persistent, autonomous behavior.

Pipeline: `video → SfM (Structure from Motion) → point cloud → 3DGS model → AI-driven animation & behavior`

- **Platform**: iOS / iPadOS (Apple platforms first)
- **License**: GPL v3
- **Languages**: C++ (core algorithms), Swift (app layer)
- **Build system**: CMake (C++), Xcode (Swift/iOS)

## Development Roadmap (5 Phases)

### Phase 1: From-scratch technical accumulation
Write SfM and 3DGS algorithms from scratch for deep understanding. Deliverables: custom SfM pipeline → sparse point cloud → dense point cloud → 3DGS training & rendering.

### Phase 2: Mature SfM integration
Replace custom SfM with COLMAP's modular components for robustness and accuracy.

### Phase 3: SwiftUI app
Build the iOS/iPadOS application shell — camera capture, Metal-based 3DGS renderer, and cat interaction UI.

### Phase 4: AI integration
Introduce AI for cat behavior — motion generation, expression, and response to user interaction.

### Phase 5: Autonomous behavior
The AI-driven cat model acts independently — wandering, playing, reacting to the environment without user input.

## Architecture

```
meowmory/
├── sfm/          # C++: Structure from Motion — extract point clouds from video
├── ios/          # Swift: iOS app shell (UI, camera, rendering)
├── cmake/        # Shared CMake modules (FindXXX.cmake, etc.)
```

### SfM module (`sfm/`)

C++ library for reconstructing 3D cat geometry from monocular video. Depends on Eigen3 for linear algebra. Outputs a point cloud that feeds into 3DGS reconstruction.

### iOS module (`ios/`)

Native Swift/SwiftUI app. Responsibilities: video capture, Metal-based 3DGS rendering, AI behavior integration, and the interaction layer.

## Build & Develop

### C++ (SfM)

```bash
# Configure (from repo root)
cmake -B build -DCMAKE_BUILD_TYPE=Release
# Build
cmake --build build
```

Requires Eigen3 installed (e.g., `brew install eigen` on macOS).

### iOS

Open the Xcode project/workspace in `ios/` and build for an iOS/iPadOS simulator or device (project file not yet created).
