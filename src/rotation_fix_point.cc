#include <cmath>
#include "imageproc.h"

#define FR_BITS 8
#define ONE_FIXP (1U << FR_BITS)

namespace imageproc {

// Forward declaration
template <size_t Depth>
static void rotate_fxp(const unsigned char *input, unsigned char *output,
                       size_t width, size_t height, float angle);

void rotate_fxp(const unsigned char *input, unsigned char *output, size_t width,
                size_t height, size_t depth, float angle) {
  if (depth == 1) {
    rotate_fxp<1U>(input, output, width, height, angle);
  } else if (depth == 3) {
    rotate_fxp<3U>(input, output, width, height, angle);
  } else {
    std::cerr << "Depth should be either 1 (grayscale) or 3 (rgb).\n";
  }
}

template <size_t Depth>
static void rotate_fxp(const unsigned char *input, unsigned char *output,
                       size_t width, size_t height, float angle) {
  size_t ld = width;

  // Indices (row, col) in the input image from where we get pixels
  int idx_fract[2];
  int idx_int[2];

  unsigned char pix00[Depth], pix01[Depth], pix10[Depth], pix11[Depth];
  // Parameters for bilinear interpolation
  int bilin[2];
  int weight[4];

  // Conversion from float to fix-point
  int sin_th = static_cast<int>(sinf(angle) * ONE_FIXP);
  int cos_th = static_cast<int>(cosf(angle) * ONE_FIXP);

  int half_width = width >> 1;
  int half_height = height >> 1;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {

      idx_fract[0] = cos_th * (row - half_height) - sin_th * (col - half_width);
      idx_fract[1] = sin_th * (row - half_height) + cos_th * (col - half_width);

      idx_int[0] = (idx_fract[0] >> FR_BITS) + half_height;
      idx_int[1] = (idx_fract[1] >> FR_BITS) + half_width;

      if ((idx_int[0] >= 0) && (idx_int[0] < (height - 1)) &&
          (idx_int[1] >= 0) && (idx_int[1] < (width - 1))) {

        bilin[0] = idx_fract[0] & (ONE_FIXP - 1);
        bilin[1] = idx_fract[1] & (ONE_FIXP - 1);

        weight[0] = (ONE_FIXP - bilin[0]) * (ONE_FIXP - bilin[1]);
        weight[1] = (ONE_FIXP - bilin[0]) * (bilin[1]);
        weight[2] = (bilin[0]) * (ONE_FIXP - bilin[1]);
        weight[3] = (bilin[0]) * (bilin[1]);

        // Will be unrolled by the compiler:
        for (int d = 0; d < Depth; d++)
          pix00[d] = input[LOC(idx_int[0], idx_int[1], ld, Depth, d)];
        for (int d = 0; d < Depth; d++)
          pix01[d] = input[LOC(idx_int[0], idx_int[1] + 1, ld, Depth, d)];
        for (int d = 0; d < Depth; d++)
          pix10[d] = input[LOC(idx_int[0] + 1, idx_int[1], ld, Depth, d)];
        for (int d = 0; d < Depth; d++)
          pix11[d] = input[LOC(idx_int[0] + 1, idx_int[1] + 1, ld, Depth, d)];

        for (int d = 0; d < Depth; d++) {
          output[LOC(row, col, ld, Depth, d)] = static_cast<unsigned char>(
              (pix00[d] * weight[0] + pix01[d] * weight[1] +
               pix10[d] * weight[2] + pix11[d] * weight[3]) >>
              (2 * FR_BITS));
        }
      }
    }
  }
}

} /* namespace imageproc */
