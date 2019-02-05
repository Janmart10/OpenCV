cmake_minimum_required(VERSION 2.8.3)
project(summer_studio)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  geometry_msgs
  roscpp
  rospy
  std_msgs
)

find_package( OpenCV REQUIRED )


## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  
  ${OpenCV_INCLUDE_DIRS}
)


catkin_package()
add_executable(main src/main.cpp src/detector.cpp src/mathutility.cpp src/timer.cpp)
target_link_libraries(main ${catkin_LIBRARIES} ${OpenCV_LIBS})


