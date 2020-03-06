cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/system/console/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(M6955Radio)
