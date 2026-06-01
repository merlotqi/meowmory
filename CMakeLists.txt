cmake_minimum_required(VERSION 3.10...3.24)
project(
  MeowmoryProject
  VERSION 0.0.1
  LANGUAGES CXX
)

find_package(Eigen3 CONFIG REQUIRED)

add_subdirectory(sfm)