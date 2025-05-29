cmake_minimum_required(VERSION 3.16)
project(parser)

set(CMAKE_CXX_STANDARD 17)

add_executable(parser
    src/parser.cpp
)

target_include_directories(parser PRIVATE src/)

install(TARGETS parser DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(PROGRAMS scripts/plot.py scripts/script.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
