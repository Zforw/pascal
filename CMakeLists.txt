cmake_minimum_required(VERSION 3.21)
project(untitled_pascal)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled_pascal main.cpp nfa_dfa.h simDFA.h minDFA.h)
