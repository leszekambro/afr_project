cmake_minimum_required(VERSION 2.8.3)
project(afr_control_reconf)

add_compile_options(-std=c++11)

find_package(X11 REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  cv_bridge
  sensor_msgs
  std_msgs
  dynamic_reconfigure
  rospy
)

find_package(Boost REQUIRED COMPONENTS system)

find_package(OpenCV)
message("OpenCV version: ${OpenCV_VERSION}")

include_directories(
	include
	${catkin_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)

LINK_DIRECTORIES("usr/local/lib")

#generate_messages(
#   DEPENDENCIES
#   mavros_msgs
#   std_msgs
#   sensor_msgs
#)

generate_dynamic_reconfigure_options(
	cfg/params.cfg
)

catkin_package(
   CATKIN_DEPENDS 
   roscpp
   cv_bridge
   sensor_msgs
   std_msgs
dynamic_reconfigure
  rospy
)


add_executable(afr_control_reconf src/Main.cpp)
add_dependencies(afr_control_reconf ${PROJECT_NAME}_gencfg)
target_link_libraries(afr_control_reconf ${catkin_LIBRARIES} ${OpenCV_LIBS} X11)
