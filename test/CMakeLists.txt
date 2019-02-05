cmake_minimum_required (VERSION 2.8)
project (imageproc_test)

find_package (PkgConfig)
pkg_check_modules (MAGICKXX REQUIRED Magick++)
string (REPLACE ";" " " MAGICKXX_CFLAGS_OTHER_STR "${MAGICKXX_CFLAGS_OTHER}")
string (REPLACE ";" " " MAGICKXX_LDFLAGS_OTHER_STR "${MAGICKXX_LDFLAGS_OTHER}")

add_executable (imageproc_test test.cc)
set_target_properties(imageproc_test PROPERTIES
  COMPILE_FLAGS "-std=c++11"
  LINK_FLAGS "${MAGICKXX_LDLFAGS_OTHER_STR}"
)
target_include_directories (imageproc_test PRIVATE ../include)
target_link_libraries (imageproc_test imageproc rawimage ${MAGICKXX_LIBRARIES})

file (COPY images DESTINATION ${CMAKE_BINARY_DIR}/test)
file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/output_images)
