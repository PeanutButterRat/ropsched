cmake_minimum_required(VERSION 3.20.0)
project(RopSched)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(${PROJECT_NAME} SHARED RopMachineScheduler.cpp Compare.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME} PRIVATE LLVM)
