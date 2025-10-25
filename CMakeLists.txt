cmake_minimum_required(VERSION 3.20.0)
project(RopSched)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED src/RopMachineScheduler.cpp src/X86CompareGadgetInstrScore.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

find_package(LLVM REQUIRED CONFIG)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${LLVM_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
)

add_definitions(${LLVM_DEFINITIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE LLVM)
