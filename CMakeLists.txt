cmake_minimum_required(VERSION 3.18)

project(literaltypes LANGUAGES CXX)

add_library(literaltypes INTERFACE)
target_compile_features(literaltypes INTERFACE cxx_std_20)

add_subdirectory(include)
add_subdirectory(examples)

