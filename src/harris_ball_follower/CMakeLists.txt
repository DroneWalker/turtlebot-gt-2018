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
        image_transport
        )

## declare and generate any messages or services

## Declare catkin package
catkin_package(
    INCLUDE_DIRS include
    LIBRARIES harris_chase_object
    CATKIN_DEPENDS geometry_msgs roscpp rospy sensor_msgs std_msgs
)


## Header files to include
add_library(find_ball_lib STATIC src/find_ball.h src/find_ball.cpp)
install(TARGETS find_ball_lib DESTINATION include)

add_library(pid_turtlebot STATIC src/pid_turtlebot.cpp)
install(TARGETS pid_turtlebot DESTINATION include)

add_library(bot_lib STATIC include/bot.h src/bot.cpp)
install(TARGETS bot_lib DESTINATION include)

## Build nodes
include_directories(include)
include_directories(include ${catkin_INCLUDE_DIRS})

#add_executable(ball_follower src/ball_follower.cpp)
#target_link_libraries(ball_follower ${OpenCV_LIBS} ${catkin_LIBRARIES})

#add_executable(find_ball src/find_orange_ball.cpp)
#target_link_libraries(find_ball ${OpenCV_LIBS} ${catkin_LIBRARIES} find_ball_lib)

add_executable(drive_wheels src/drive_wheels.cpp)
target_link_libraries(drive_wheels ${catkin_LIBRARIES} pid_turtlebot)

#add_executable(ball_follower_debug src/ball_follower_debug.cpp)
#target_link_libraries(ball_follower_debug ${OpenCV_LIBS} ${catkin_LIBRARIES})

add_executable(commander src/commander_node.cpp)
target_link_libraries(commander bot_lib ${catkin_LIBRARIES})

#add_executable(circle_test bin/contour_circle_test.cpp)
#target_link_libraries(circle_test ${OpenCV_LIBS} find_ball_lib)

add_executable(find_ball_node src/find_orange_ball_node.cpp)
target_link_libraries(find_ball_node ${OpenCV_LIBS} ${catkin_LIBRARIES} find_ball_lib)






##add_executable(compress_test src/compress_test.cpp)
##target_link_libraries(compress_test ${OpenCV_LIBS} ${catkin_LIBRARIES})