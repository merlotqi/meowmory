# Gaussian Scale-Space Pyramid

> Reference for SfM Spiral 2 — understanding the mathematical foundation behind
> SIFT's keypoint detector.

## 1. Why Scale-Space?

Real-world objects appear at different scales depending on their distance from the
camera. A feature detector that only works at one scale will miss the same structure
when it appears larger or smaller. **Scale-space theory** solves this by representing
the image as a family of progressively smoothed versions, parameterized by a
continuous scale parameter σ.

The key insight: **structures that persist across multiple scales are likely "real",
while those that appear at only one scale are likely noise.**

## 2. The Gaussian Kernel

Among all smoothing kernels, the Gaussian is special. It is the **unique kernel**
that satisfies the scale-space axioms:

- **Causality**: coarser scales contain only simpler structures than finer scales.
  No new structure is created as σ increases.
- **Linearity**: the transformation from fine to coarse scale is linear.
- **Isotropy**: smoothing treats all directions equally.
- **Semi-group property**: convolving with σ₁ then σ₂ is equivalent to convolving
  once with σ = √(σ₁² + σ₂²).

The 2D Gaussian kernel:

```
G(x, y, σ) = (1 / 2πσ²) · exp( -(x² + y²) / (2σ²) )
```

Convolving an image L(x,y) with G(x,y,σ) produces the scale-space representation:

```
L(x, y, σ) = G(x, y, σ) ∗ I(x, y)
```

Where `∗` denotes convolution.

### The Semi-Group Property in Practice

Given an image blurred to σ₁, to reach σ₂ where σ₂ > σ₁, convolve with:

```
σ_blur = √(σ₂² - σ₁²)
```

This avoids re-blurring from scratch and is used throughout pyramid construction.

## 3. The Scale-Space Pyramid

### 3.1 Why a Pyramid?

A full scale-space at the original resolution for many σ values is computationally
expensive. The pyramid approach is more efficient:

- **Within an octave**: progressively blur with increasing σ at fixed resolution.
- **Between octaves**: downsample by 2×, then continue blurring. This saves
  computation and naturally handles larger scales.

### 3.2 Octave and Scale Structure

Each **octave** represents a doubling of σ. Within an octave, the scale is sampled
at discrete intervals:

```
σ(s) = σ₀ · k^s    where k = 2^(1/n_scales)    and s = 0, 1, 2, ...
```

| Parameter | Typical value | Meaning |
|-----------|--------------|---------|
| σ₀ | 1.6 | Base scale (Lowe's choice) |
| n_scales | 3 | Intervals per octave |
| k | 2^(1/3) ≈ 1.26 | Multiplicative step between adjacent scales |
| n_images | n_scales + 3 = 6 | Blurred images needed per octave |

The σ values at the finest octave:
```
s=0: σ₀        = 1.60
s=1: σ₀·k      ≈ 2.02
s=2: σ₀·k²     ≈ 2.54
s=3: σ₀·k³     ≈ 3.20    ← σ doubles from s=0 to s=3
s=4: σ₀·k⁴     ≈ 4.03
s=5: σ₀·k⁵     ≈ 5.08
```

### 3.3 Why n_scales + 3 Images?

Extrema detection requires a 3×3×3 neighborhood. For the middle DoG level s, we need:

```
DoG(s-1), DoG(s), DoG(s+1)
```

DoG(s) = L(s+1) - L(s), so we need L(s-1)..L(s+2) = 4 adjacent Gaussian images
for **one** usable DoG level. With n_scales usable DoG levels:

```
n_images_needed = n_scales + 3
```

For n_scales=3: we need 6 Gaussian images, producing 5 DoG images, of which 3
are usable for extrema detection (indices 1, 2, 3 — each has neighbors above
and below).

### 3.4 Downsampling Between Octaves

When moving from octave `o` to octave `o+1`:

1. Take the blurred image at `s = n_scales` (i.e., σ = σ₀ · 2 = 2σ₀)
2. Downsample by 2×
3. This downsampled image has effective σ = σ₀ in the new octave's coordinates

This works because: downsampling by 2 halves the image dimensions and doubles the
effective σ relative to pixel size. What was 2σ₀ at full resolution becomes σ₀
in the halved coordinates.

## 4. Difference of Gaussians (DoG)

### 4.1 Approximation to Laplacian of Gaussian

The **Laplacian of Gaussian** (LoG) produces strong responses at blob-like
structures. However, computing LoG at every scale is expensive.

The **Difference of Gaussians** approximates LoG efficiently:

```
D(x, y, σ) = L(x, y, kσ) - L(x, y, σ)
            ≈ (k-1) · σ² · ∇²G ∗ I
```

The approximation comes from the heat diffusion equation:

```
∂G/∂σ = σ · ∇²G

∂G/∂σ ≈ [G(x,y,kσ) - G(x,y,σ)] / (kσ - σ)

∴ G(x,y,kσ) - G(x,y,σ) ≈ (k-1)σ² · ∇²G
```

The factor (k-1) is a constant scaling that doesn't affect extrema locations.

### 4.2 Why DoG Works Well

- **Efficiency**: just subtract adjacent blurred images (no Laplacian computation).
- **Scale-normalized**: the σ² factor approximates true scale normalization for
  the Laplacian, so blob responses are comparable across scales.
- **Stability**: DoG extrema are more stable than gradient-based or Hessian-based
  detectors under common image transformations.

## 5. Keypoint Localization in the DoG Pyramid

### 5.1 Extrema Detection

A pixel (x, y, σ) in the DoG pyramid is an extremum if it is strictly greater
than (or less than) all **26 neighbors** in the 3×3×3 cube:

```
  DoG(s-1):  [nb nb nb]    DoG(s):  [nb nb nb]    DoG(s+1):  [nb nb nb]
             [nb nb nb]             [nb xx nb]               [nb nb nb]
             [nb nb nb]             [nb nb nb]               [nb nb nb]
                                                          xx = candidate
```

Border pixels (row=0, row=max, col=0, col=max) are skipped since they lack
a full 3×3 neighborhood.

### 5.2 Subpixel Refinement (Taylor Expansion)

The discrete sample point is not the true extremum. To find the true location,
fit a 3D quadratic function around the candidate:

```
D(x) ≈ D + (∂D/∂x)^T · x + ½ x^T · (∂²D/∂x²) · x

where x = (x, y, σ)^T is the offset from the sample point
```

Take the derivative and set to zero:

```
x̂ = - (∂²D/∂x²)^(-1) · (∂D/∂x)
```

Solve the 3×3 linear system. Reject if |x̂| > 0.5 in any dimension (the true
peak is closer to a different sample point, and we'd find it there instead).

### 5.3 Edge Suppression (Hessian Criterion)

DoG has strong responses along edges, but these are poorly localized (the
location along the edge is ambiguous). A keypoint on a true corner/blop has
**two principal curvatures of similar magnitude**; an edge has **one large
and one small curvature**.

The principal curvatures are proportional to the eigenvalues of the 2×2 Hessian:

```
H = [Dxx  Dxy]
    [Dxy  Dyy]
```

Let α = larger eigenvalue, β = smaller eigenvalue, r = α/β:

```
Tr(H) = Dxx + Dyy = α + β
Det(H) = Dxx·Dyy - Dxy² = α·β

Tr(H)² / Det(H) = (α+β)²/(αβ) = (r+1)²/r
```

This ratio is minimized when r=1 (α=β, isotropic corner) and grows with r
(anisotropic edge). Reject the keypoint if:

```
Tr(H)² / Det(H) > (r_thresh + 1)² / r_thresh
```

Lowe uses r_thresh = 10, rejecting keypoints with principal curvature ratio > 10.

Note: if Det(H) ≤ 0, the eigenvalues have opposite signs (saddle point) → reject.

## 6. Coordinate System

### Input → Octave Mapping

```
Octave 0:  input image is doubled  (×2) before processing
Octave 1:  input at original resolution
Octave 2:  input downsampled by 2
Octave k:  input downsampled by 2^(k-1) (relative to original)
```

### Converting Back to Input Coordinates

For a keypoint found at (col, row) in octave `o`'s coordinate system:

```
img_x = col × 2^(o-1)
img_y = row × 2^(o-1)
sigma = σ₀·k^s × 2^(o-1)
```

The initial ×2 upsampling of octave 0 means its coordinates must be divided by 2
to map back to the input image.

## 7. Summary of Parameters

| Parameter | Lowe's value | Role |
|-----------|-------------|------|
| σ₀ | 1.6 | Base scale (assumes input image has nominal σ ≈ 0.5) |
| n_scales | 3 | DoG levels usable for extrema detection per octave |
| n_octaves | ⌊log₂(min(w,h))⌋ - 3 | Auto-computed from image size |
| k | 2^(1/n_scales) | Scale step multiplier |
| contrast_thresh | 0.03 | Reject low-contrast keypoints (normalized [0,1]) |
| edge_ratio_thresh | 10.0 | Reject keypoints with high edge-ness |
| max_refine_iters | 5 | Taylor refinement iterations |
| offset_max | 0.5 | Max subpixel offset per dimension |

## 8. Further Reading

- Lowe, D.G. (2004). "Distinctive Image Features from Scale-Invariant Keypoints."
  *International Journal of Computer Vision*, 60(2), 91-110.
- Lindeberg, T. (1994). "Scale-Space Theory: A Basic Tool for Analysing
  Structures at Different Scales." *Journal of Applied Statistics*, 21(2), 225-270.
- OpenCV `cv::SIFT` source: `modules/features2d/src/sift.dispatch.cpp`
