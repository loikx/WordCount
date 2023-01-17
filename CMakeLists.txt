cmake_minimum_required(VERSION 3.0.0)
project(labwork1 VERSION 0.1.0)

add_executable(labwork1 main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "WordCount")
