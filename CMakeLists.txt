cmake_minimum_required(VERSION 3.7)
project(DBProject)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES
        filesystem/testfilesystem.cpp
        filesystem/*/*.h
        )

add_executable(DBProject ${SOURCES})