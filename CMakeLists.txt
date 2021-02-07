#
#
cmake_minimum_required(VERSION 3.7)
project(pulseDect)
set(PROJECT_VERSION "0.01")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")
set(PULSEDET_SRC src/pulseDetect.cc src/HW.cc)
add_executable(pulseDetect ${PULSEDET_SRC})
target_link_libraries(pulseDetect pthread wiringPi)
