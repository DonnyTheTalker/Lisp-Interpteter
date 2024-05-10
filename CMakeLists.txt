cmake_minimum_required(VERSION 3.28)
project(lisp)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp heap.cpp object.cpp parser.cpp lisp.cpp tokenizer.cpp)
set(HEADER_FILES heap.h error.h object.h parser.h lisp.h tokenizer.h)

add_executable(lisp_int ${SOURCE_FILES})
