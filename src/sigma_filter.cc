#include <algorithm>
#include "imageproc.h"

namespace imageproc {

// Forward declaration
template <size_t Depth>
static void
sigma_filter(const unsigned char *input, unsigned char *output, size_t width,
             size_t height, unsigned char sigma,
             size_t kernel_size // kernel width == height == 2*kern_size + 1
             );

void
sigma_filter(const unsigned char *input, unsigned char *output, size_t width,
             size_t height, size_t depth, unsigned char sigma,
             size_t kernel_size // kernel width == height == 2*kern_size + 1
             ) {
  if (depth == 1) {
    sigma_filter<1U>(input, output, width, height, sigma, kernel_size);
  } else if (depth == 3) {
    sigma_filter<3U>(input, output, width, height, sigma, kernel_size);
  } else
    std::cerr << "Depth should be either 1 (grayscale) or 3 (rgb).\n";
}

// Helper predicate for assertions
template <typename T> static bool all_non_negative(T a[], size_t s) {
  for (int i = 0; i < s; i++)
    if (a[i] < 0)
      return false;
  return true;
}

template <size_t Depth>
static void
sigma_filter(const unsigned char *input, unsigned char *output, size_t width,
             size_t height, unsigned char sigma,
             size_t kernel_size // kernel width == height == 2*kern_size + 1
             ) {
  int ymin, ymax;
  std::uint32_t hist[Depth][256]; // Local histogram
  int ld = width;                 // Row-major memory layout

  int row_min, row_max, col_minus, col_plus;
  unsigned char pix_min, pix_max, pix_val;
  std::uint32_t sum = 0, n = 0;
  int kern_size = static_cast<int>(kernel_size);

  for (int row = 0; row < height; row++) {

    row_min = std::max(0, row - kern_size);
    row_max = std::min(static_cast<int>(height) - 1, row + kern_size);

    for (int col = 0; col < width; col++) {
      col_minus = col - kern_size - 1;
      col_plus = col + kern_size;

      if (col == 0) { // Hist init
        for (int d = 0; d < Depth; d++) {
          for (int i = 0; i < 256; i++)
            hist[d][i] = 0;
        }

        for (int r = row_min; r <= row_max; r++) {
          for (int c = 0; c <= col_plus; c++) {
            for (int d = 0; d < Depth; d++) {
              hist[d][input[LOC(r, c, ld, Depth, d)]]++;
            }
          }
        }
      } else {

        if (col_minus >= 0) {
          for (int r = row_min; r <= row_max; r++) {
            for (int d = 0; d < Depth; d++) {
              hist[d][input[LOC(r, col_minus, ld, Depth, d)]]--;
            }
          }
        }

        if (col_plus < width) {
          for (int r = row_min; r <= row_max; r++) {
            for (int d = 0; d < Depth; d++) {
              hist[d][input[LOC(r, col_plus, ld, Depth, d)]]++;
            }
          }
        }
      }

      for (int d = 0; d < Depth; d++) {
        assert(all_non_negative(&hist[d][0], 256)); // Invariant
        sum = 0;
        n = 0;
        pix_val = input[LOC(row, col, ld, Depth, d)];
        pix_min = std::max(0, pix_val - sigma);
        pix_max = std::min(255, pix_val + sigma);

        for (std::uint32_t p_val = pix_min; p_val <= pix_max; p_val++) {
          sum += p_val * hist[d][p_val];
          n += hist[d][p_val];
        }

        output[LOC(row, col, ld, Depth, d)] = static_cast<unsigned char>(
            (n > 0) ? ((sum + (n >> 1)) / n) : pix_val);
      }
    }
  }
}

} /* namespace imageproc */
