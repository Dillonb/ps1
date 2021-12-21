cmake_minimum_required (VERSION 3.10)

project (dgb-ps1)
set(CMAKE_CXX_STANDARD 20)
set(PS1_TARGET ps1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
include(CTest)
add_subdirectory(src)