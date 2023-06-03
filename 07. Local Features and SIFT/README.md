# Local Feature Detection and Matching

## Feature Detection Procedure

1. Find a set of **distinctive keypoints** (e.g. corners, blobs)
2. Define a **supporting region** around each keypoint in a scale or affine invariant manner
3. Extract and **normalize** the region content
4. Compute a **descriptor** from the normalized region
5. **Match** the local descriptors

## Example of Local Features

- Edges
- **Corners** (e.g., Harris)
  - Location + orientation
- Blobs (e.g., SIFT)
  - Corner + scale

## Corner Features

- Points in 2D image with high curvature
- Doesn't necessarily correspond to real 3D corner
- Take place at **junctions**, highly **textured** surface, **occlusion boundary**, etc.

### Corner Features - Why?

- Corner points are **stable** feature
  - 변형이 생겨도 corner point가 바뀌지 않음
  - local한 관점에서 uniqu함으로 특징을 표현하는데 적합함
- Robust to **lighting** and **viewpoint** changes
- Position shifting from the corner yields large **variation** in appearance

### Harris Corner Detector

Change of intensity for the shift $\Delta \mathbf{u}$ at $I(x, y)$:
$$E_{AC}(\Delta \mathbf{u}) = \sum_{i} w(\mathbf{x_i}) [I_0(\mathbf{x_i} + \Delta \mathbf{u}) - I_0(\mathbf{x_i})]^2$$

- $\Delta \mathbf{u}$: shift의 양
- $I_0(\mathbf{x_i})$: intensity at $\mathbf{x_i}$
  - shift 되기 전 window
- $I_0(\mathbf{x_i} + \Delta \mathbf{u})$: Shifted intensity
  - shift 되고 난 후 window
- $w(\mathbf{x_i})$: Window function (e.g., Gaussian)
