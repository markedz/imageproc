add_library (rawimage rawimage.cc)
set_target_properties(rawimage PROPERTIES
  COMPILE_FLAGS "-std=c++11 ${MAGICKXX_CFLAGS_OTHER_STR}"
  LINK_FLAGS "${MAGICKXX_LDLFAGS_OTHER_STR}"
)
target_include_directories (rawimage PRIVATE ../include PUBLIC ${MAGICKXX_INCLUDE_DIRS})
target_link_libraries (rawimage ${MAGICKXX_LIBRARIES})

add_library (imageproc rotation.cc rotation_fix_point.cc sigma_filter.cc)
set_target_properties(imageproc PROPERTIES
  COMPILE_FLAGS "-std=c++11"
)
target_include_directories (imageproc PRIVATE ../include)
target_link_libraries (imageproc rawimage)
