cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(multimeterServer LANGUAGES CXX)

set(DEIRECTORIES
    ./src
)

add_subdirectory(src)
