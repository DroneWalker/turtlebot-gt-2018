# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "harris_chase_object: 1 messages, 0 services")

set(MSG_I_FLAGS "-Iharris_chase_object:/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg;-Istd_msgs:/opt/ros/kinetic/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(harris_chase_object_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_custom_target(_harris_chase_object_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "harris_chase_object" "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" ""
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(harris_chase_object
  "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/harris_chase_object
)

### Generating Services

### Generating Module File
_generate_module_cpp(harris_chase_object
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/harris_chase_object
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(harris_chase_object_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(harris_chase_object_generate_messages harris_chase_object_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_dependencies(harris_chase_object_generate_messages_cpp _harris_chase_object_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(harris_chase_object_gencpp)
add_dependencies(harris_chase_object_gencpp harris_chase_object_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS harris_chase_object_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(harris_chase_object
  "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/harris_chase_object
)

### Generating Services

### Generating Module File
_generate_module_eus(harris_chase_object
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/harris_chase_object
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(harris_chase_object_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(harris_chase_object_generate_messages harris_chase_object_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_dependencies(harris_chase_object_generate_messages_eus _harris_chase_object_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(harris_chase_object_geneus)
add_dependencies(harris_chase_object_geneus harris_chase_object_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS harris_chase_object_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(harris_chase_object
  "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/harris_chase_object
)

### Generating Services

### Generating Module File
_generate_module_lisp(harris_chase_object
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/harris_chase_object
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(harris_chase_object_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(harris_chase_object_generate_messages harris_chase_object_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_dependencies(harris_chase_object_generate_messages_lisp _harris_chase_object_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(harris_chase_object_genlisp)
add_dependencies(harris_chase_object_genlisp harris_chase_object_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS harris_chase_object_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(harris_chase_object
  "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/harris_chase_object
)

### Generating Services

### Generating Module File
_generate_module_nodejs(harris_chase_object
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/harris_chase_object
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(harris_chase_object_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(harris_chase_object_generate_messages harris_chase_object_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_dependencies(harris_chase_object_generate_messages_nodejs _harris_chase_object_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(harris_chase_object_gennodejs)
add_dependencies(harris_chase_object_gennodejs harris_chase_object_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS harris_chase_object_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(harris_chase_object
  "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/harris_chase_object
)

### Generating Services

### Generating Module File
_generate_module_py(harris_chase_object
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/harris_chase_object
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(harris_chase_object_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(harris_chase_object_generate_messages harris_chase_object_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/charris/introrobotics/turtlebot-gt-2018/src/harris_chase_object/msg/DistanceAngle.msg" NAME_WE)
add_dependencies(harris_chase_object_generate_messages_py _harris_chase_object_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(harris_chase_object_genpy)
add_dependencies(harris_chase_object_genpy harris_chase_object_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS harris_chase_object_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/harris_chase_object)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/harris_chase_object
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(harris_chase_object_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/harris_chase_object)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/harris_chase_object
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(harris_chase_object_generate_messages_eus std_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/harris_chase_object)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/harris_chase_object
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(harris_chase_object_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/harris_chase_object)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/harris_chase_object
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(harris_chase_object_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/harris_chase_object)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/harris_chase_object\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/harris_chase_object
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(harris_chase_object_generate_messages_py std_msgs_generate_messages_py)
endif()
