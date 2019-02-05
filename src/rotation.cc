#include <cmath>
#include "imageproc.h"

namespace imageproc {

// Forward declaration
template <size_t Depth>
static void rotate(const unsigned char *input, unsigned char *output,
                   size_t width, size_t height, float angle);

void rotate(const unsigned char *input, unsigned char *output, size_t width,
            size_t height, size_t depth, float angle) {
  if (depth == 1) {
    rotate<1U>(input, output, width, height, angle);
  } else if (depth == 3) {
    rotate<3U>(input, output, width, height, angle);
  } else {
    std::cerr << "Depth should be either 1 (grayscale) or 3 (rgb).\n";
  }
}

template <size_t Depth>
static void rotate(const unsigned char *input, unsigned char *output,
                   size_t width, size_t height, float angle) {
  size_t ld = width;

  // Indices (row, col) in the input image from where we get pixels
  float idx_fract[2];
  float idx_fract_round[2];
  int idx_int[2];

  float pix00[Depth], pix01[Depth], pix10[Depth], pix11[Depth];
  // Parameters for bilinear interpolation
  float bilin[2];
  float weight[4];

  float sin_th = sinf(angle);
  float cos_th = cosf(angle);

  int half_width = width >> 1;
  int half_height = height >> 1;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {

      idx_fract[0] = cos_th * (static_cast<float>(row - half_height)) -
                     sin_th * (static_cast<float>(col - half_width));
      idx_fract[1] = sin_th * (static_cast<float>(row - half_height)) +
                     cos_th * (static_cast<float>(col - half_width));

      idx_fract_round[0] = floorf(idx_fract[0]); // or use modff
      idx_fract_round[1] = floorf(idx_fract[1]);

      idx_int[0] = static_cast<int>(idx_fract_round[0]) + half_height;
      idx_int[1] = static_cast<int>(idx_fract_round[1]) + half_width;

      if ((idx_int[0] >= 0) && (idx_int[0] < (height - 1)) &&
          (idx_int[1] >= 0) && (idx_int[1] < (width - 1))) {

        bilin[0] = idx_fract[0] - idx_fract_round[0];
        bilin[1] = idx_fract[1] - idx_fract_round[1];

        weight[0] = (1. - bilin[0]) * (1. - bilin[1]);
        weight[1] = (1. - bilin[0]) * (bilin[1]);
        weight[2] = (bilin[0]) * (1. - bilin[1]);
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
              static_cast<float>(pix00[d]) * weight[0] +
              static_cast<float>(pix01[d]) * weight[1] +
              static_cast<float>(pix10[d]) * weight[2] +
              static_cast<float>(pix11[d]) * weight[3]);
        }
      }
    }
  }
}

} /* namespace imageproc */
