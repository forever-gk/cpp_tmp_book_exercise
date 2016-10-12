cmake_minimum_required(VERSION 3.5.2)
project(cpp_tmp_book_exercise)

set(BOOST_ROOT "../boost_1_62_0")
find_package(Boost 1.62.0)
if(Boost_FOUND)
    include_directories(${BOOST_ROOT})
else()
    message(FATAL_ERROR "Could not find boost!")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

set(SOURCE_FILES
        main.cpp
        binary.h
        tree.h
        appendix_a.h
        chapter_02.cpp
        chapter_03.cpp
        chapter_04.cpp
        chapter_05.cpp
        chapter_06.cpp
        chapter_07.cpp
        chapter_09.cpp
        chapter_10.cpp
        appendix_a.cpp)

add_executable(cpp_tmp_book_exercise ${SOURCE_FILES})

