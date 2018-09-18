cmake_minimum_required(VERSION 2.8.3)
project(harris_ball_follower)
find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
        roscpp
        rospy
        std_msgs
        sensor_msgs
        std_msgs
        geometry_msgs
        cv_bridge
        )

## declare and generate any messages or services

## Declare catkin package
catkin_package()

## Build nodes
include_directories(include)
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(ball_follower src/ball_follower.cpp)
target_link_libraries(ball_follower ${OpenCV_LIBS} ${catkin_LIBRARIES})

add_executable(drive_wheels src/drive_wheels.cpp)
target_link_libraries(drive_wheels ${catkin_LIBRARIES})



##add_executable(compress_test src/compress_test.cpp)
##target_link_libraries(compress_test ${OpenCV_LIBS} ${catkin_LIBRARIES})