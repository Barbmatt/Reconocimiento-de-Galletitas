cmake_minimum_required(VERSION 2.8)
project( Galletitas )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( Galletitas Galletitas.cpp )
target_link_libraries( Galletitas ${OpenCV_LIBS} )
