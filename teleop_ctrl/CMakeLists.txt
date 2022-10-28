cmake_minimum_required(VERSION 3.0.2)
project(teleop_ctrl)

# add_compile_options(-std=c++11)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
  genmsg
  joy
)


## Declare ROS messages and services
 #add_message_files(
 #  FILES
 #  Num.msg
 #)

 #add_service_files(
  # FILES
  # AddTwoInts.srv
 #)


## Generate added messages and services
 generate_messages(
   DEPENDENCIES
   std_msgs
 )


## Declare a catkin package
catkin_package(
  CATKIN_DEPENDS roscpp std_msgs message_runtime joy
)

## Build talker and listener
include_directories(
   include ${catkin_INCLUDE_DIRS}
)

add_executable(teleop_ctrl src/teleop_ctrl.cpp )
target_link_libraries(teleop_ctrl ${catkin_LIBRARIES})
add_dependencies(teleop_ctrl teleop_ctrl_generate_messages_cpp)

add_executable(botFollower src/botFollower.cpp)
target_link_libraries(botFollower ${catkin_LIBRARIES})
#add_dependencies(botFollower botFollower_generate_messages_cpp)



