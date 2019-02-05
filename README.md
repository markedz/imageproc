# Image Processing

This is a collection of image processing routines implemented with efficiency in mind.
Currently the following algorithms are provided:

* sigma filter (using a local histogram for performance)
* plane rotation of an image relative to the center (with bilinear interpolation):
  * floating-point version
  * fixed-point version

For running tests of the above on sample images see the end of this document.

## Getting started

### Prerequisites

Following software components must be installed in the system:

* recent C++ compiler (with C++11 support)
* cmake 3.x or newer
* pkgconfig
* Magick++ library (lightweight library for reading and writing image files)


### Building

Example shell commands executed in the project directory that manage the building process:

```
mkdir build
cd build
cmake ..
make
```

### Running tests

After the building process is finished, the following command can be used to execute the tests:

```
make check
```

Test output images can be found in your_build_dir/test/output_images
