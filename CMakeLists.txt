# # cmake_minimum_required(VERSION 3.12)
# # project(brain_cpp)

# # set(CMAKE_CXX_STANDARD 14)

# # # Find Python and pybind11 package
# # find_package(Python COMPONENTS Interpreter Development)
# # find_package(pybind11 REQUIRED)

# # # Include directories
# project(brain_cpp)
# cmake_minimum_required(VERSION 3.10)

# add_subdirectory(ThirdParty/pybind11)

# pybind11_add_module(brain_cpp brain.cpp)

# target_link_libraries(brain_cpp yaml-cpp)

cmake_minimum_required(VERSION 3.10)
project(brain_cpp)

add_subdirectory(ThirdParty/pybind11)

pybind11_add_module(brain_cpp MODULE brain.cpp)

target_link_libraries(brain_cpp PRIVATE yaml-cpp)
