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

        )

#add_executable(find_ball src/find_ball.cpp src/ball_follower.cpp)
#target_link_libraries(find_ball ${catkin_LIBRARIES})

#add_executable(image_publisher src/image_publisher.cpp)
#target_link_libraries(image_publisher ${catkin_LIBRARIES})

add_executable(ball_follower src/ball_follower.cpp)
target_link_libraries(ball_follower ${OpenCV_LIBS})

include_directories(include ${catkin_INCLUDE_DIRS})




