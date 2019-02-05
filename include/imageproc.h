#ifndef __IMAGEPROC_H
#define __IMAGEPROC_H

#include <cstdint>
#include <cstddef>  // size_t
#include <cassert>  // assert
#include <iostream> // std::cerr

#define LOC(row, col, ld, depth, d) (((row) * (ld) + (col)) * (depth) + (d))

namespace imageproc {

void
sigma_filter(const unsigned char *input, unsigned char *output, size_t width,
             size_t height, size_t depth, unsigned char sigma,
             size_t kernel_size = 1 // kernel width == height == 2*kern_size + 1
             );

void rotate(const unsigned char *input, unsigned char *output, size_t width,
            size_t height, size_t depth, float angle);

void rotate_fxp(const unsigned char *input, unsigned char *output, size_t width,
                size_t height, size_t depth, float angle);

} /* namespace imageproc */

#endif /* __IMAGEPROC_H */
