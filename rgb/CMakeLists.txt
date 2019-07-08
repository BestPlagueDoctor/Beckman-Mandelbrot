# Setup #
cmake_minimum_required(VERSION 3.9)
project(Mandelbrot)
set(CMAKE_BUILD_TYPE Release)

# Variables #
set(CMAKE_CXX_FLAGS "-march=native -fopenmp -Wshadow -Wall -Wextra -pedantic -pipe -Ofast -fno-signed-zeros -freciprocal-math -ffp-contract=fast")
set(CMAKE_CXX_STANDARD 17)

include_directories(include third_party)
file(GLOB CXX_SRCS "src/*.cpp")
add_executable(mandelbrot ${CXX_SRCS})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
