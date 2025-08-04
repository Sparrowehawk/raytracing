cmake_minimum_required(VERSION 3.10)

# Project name and language
project(ImageGenerator LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable from main.cpp
add_executable(image_generator main.cpp)
