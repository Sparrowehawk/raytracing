cmake_minimum_required(VERSION 3.10)

# Project name and language
project(ImageGenerator LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenMP REQUIRED)

# Add executable from main.cpp
add_executable(image_generator main.cpp)

target_link_libraries(image_generator PRIVATE OpenMP::OpenMP_CXX)

# Set a default build type to "Release" if none is specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Add compiler flags for Release builds
# These flags are for maximum optimization (-O3) and targeting your specific CPU architecture (-march=native)
target_compile_options(image_generator PRIVATE
  $<$<CONFIG:Release>:-O3 -march=native>
)