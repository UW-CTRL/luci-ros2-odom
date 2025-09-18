cmake_minimum_required(VERSION 3.8)
project(luci_encoder_odometry)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(luci_messages REQUIRED)

# Tell CMake where to find headers
include_directories(include)

# Add executable (note: file is encoder_to_odom.cpp, not encoder_odom_node.cpp)
add_executable(encoder_to_odom_node
  src/encoder_to_odom.cpp
  src/odometry.cpp
)

ament_target_dependencies(encoder_to_odom_node
  rclcpp
  nav_msgs
  geometry_msgs
  tf2
  tf2_ros
  luci_messages
)

install(TARGETS
  encoder_to_odom_node
  DESTINATION lib/${PROJECT_NAME})
