cmake_minimum_required(VERSION 3.5)

project(multimeterServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

set(SOURCES
    core/MultimeterServer.cpp
    core/MultimeterServer.h
    core/ClientHandler.cpp
    core/ClientHandler.h
    core/MultimeterDispatcher.cpp
    core/MultimeterDispatcher.h
    core/RequestHandler.cpp
    core/RequestHandler.h
    main.cpp
    ${UsingApplicationPlatform}
)

add_executable(multimeterServer ${SOURCES})
