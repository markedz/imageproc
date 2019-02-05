cmake_minimum_required (VERSION 3.0)
project (imageproc_test)

find_package (PkgConfig)
pkg_check_modules (MAGICKXX REQUIRED Magick++)
string (REPLACE ";" " " MAGICKXX_CFLAGS_OTHER_STR "${MAGICKXX_CFLAGS_OTHER}")
string (REPLACE ";" " " MAGICKXX_LDFLAGS_OTHER_STR "${MAGICKXX_LDFLAGS_OTHER}")

add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (test)

add_custom_target(check ${CMAKE_COMMAND} -E remove * COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/test/images/* . COMMAND ${CMAKE_BINARY_DIR}/test/imageproc_test * DEPENDS imageproc_test WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/output_images)
