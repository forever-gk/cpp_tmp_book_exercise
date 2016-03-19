cmake_minimum_required(VERSION 3.3)
project(cpp_tmp_book_exercise)

set(BOOST_ROOT "../boost_1_60_0")
find_package(Boost 1.60.0)
if(Boost_FOUND)
    include_directories(${BOOST_ROOT})
else()
    message(FATAL_ERROR "Could not find boost!")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp chapter_02.cpp chapter_03.cpp chapter_04.cpp chapter_05.cpp chapter_06.cpp)

add_executable(cpp_tmp_book_exercise ${SOURCE_FILES})

