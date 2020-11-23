cmake_minimum_required(VERSION 3.17)
project(EulerOperator)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL -framework GLUT")
set(CMAKE_CXX_STANDARD 14)

add_executable(EulerOperator main.cpp euler_operator.cpp euler_operator.h half_edge_data_structure.h sweep.cpp sweep.h renderer.cpp renderer.h)

include_directories("/usr/local/include")