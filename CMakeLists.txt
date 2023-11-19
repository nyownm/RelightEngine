cmake_minimum_required(VERSION 3.26)
project(RelightEngine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GlobalOptions)

add_subdirectory(ThirdParty)

add_subdirectory(Source)

add_subdirectory(Tests)