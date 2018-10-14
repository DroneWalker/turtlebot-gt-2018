cmake_minimum_required(VERSION 2.8.3)
project(harris_chase_object)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
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
        message_generation
        )

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################
add_message_files(
        FILES
        DistanceAngle.msg
)

generate_messages(
        DEPENDENCIES
        std_msgs
)


###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES harris_chase_object
  CATKIN_DEPENDS geometry_msgs roscpp rospy sensor_msgs std_msgs message_runtime
)

###########
## Build ##
###########


## Header files to include
add_library(find_object_lib STATIC src/objects/find_ball.cpp)
install(TARGETS find_object_lib DESTINATION include)

add_library(pid_lib STATIC src/objects/pid_turtlebot.cpp)
install(TARGETS pid_lib DESTINATION include)

add_library(thisbot_lib STATIC src/objects/bot.cpp)
install(TARGETS thisbot_lib DESTINATION include)

add_library(object_lib STATIC src/objects/object.cpp)
install (TARGETS object_lib DESTINATION include)

## Build nodes
include_directories(include)
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(detect_object src/nodes/detect_ball_node.cpp)
target_link_libraries(detect_object ${OpenCV_LIBS} ${catkin_LIBRARIES} find_object_lib)

#add_executable(drive_bot src/nodes/drive_wheels_node.cpp)
#target_link_libraries(drive_bot ${catkin_LIBRARIES} pid_lib)

add_executable(follow_drive src/nodes/follow_drive_node.cpp)
target_link_libraries(follow_drive ${catkin_LIBRARIES} pid_lib)

add_executable(command src/nodes/commander_node.cpp)
target_link_libraries(command ${catkin_LIBRARIES} thisbot_lib object_lib)
add_dependencies(command ${${PROJECT_NAME}_EXPORTED_TARGETS})

#add_executable(fake_scan src/fakes/fake_laser_Scan.cpp)
#target_link_libraries(fake_scan ${catkin_LIBRARIES})

