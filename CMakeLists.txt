cmake_minimum_required(VERSION 3.8)
project(Fays_VI)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(std_msgs REQUIRED)
find_package(Threads REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

# OpenCV library path
set(OPENCV_LOCAL_DIR "${CMAKE_SOURCE_DIR}/thirdparty/opencv-4.2.0-linux-x86_64")
set(OpenCV_DIR "${OPENCV_LOCAL_DIR}/lib/cmake/opencv4")
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
# find_package(OpenCV REQUIRED)


# FAYS_ATRAK library
set(FAYS_ATRAK_LIB_DIR "${CMAKE_SOURCE_DIR}/thirdparty/fays_atrak/${CMAKE_SYSTEM_PROCESSOR}/Release" CACHE PATH "Path to fays_atrak libraries")
message(STATUS "FAYS_ATRAK_LIB_DIR: ${FAYS_ATRAK_LIB_DIR}")
# FTDI library path
set(FTDPATH "${CMAKE_SOURCE_DIR}/thirdparty/ft602-linux-x86_64")


# Find FAYS_ATRAK and FTDI libraries
find_library(FAYS_ATRAK_VI_MODULE_LIB
    NAMES fays_atrak_vi_module
    PATHS ${FAYS_ATRAK_LIB_DIR}
    NO_DEFAULT_PATH
    REQUIRED
)
find_library(FTD3XX_LIBRARY ft602 PATHS ${FTDPATH} NO_DEFAULT_PATH REQUIRED)
message(STATUS "FTD3XX_LIBRARY: ${FTD3XX_LIBRARY}")
if(NOT FAYS_ATRAK_VI_MODULE_LIB)
    message(FATAL_ERROR "Failed to find libfays_atrak_vi_module.so")
endif()
if(NOT FTD3XX_LIBRARY)
    message(FATAL_ERROR "ft602 library not found")
endif()

add_executable(Fays_VI_node src/Fays_VI_node.cpp)
ament_target_dependencies(Fays_VI_node
  rclcpp
  sensor_msgs
  std_msgs
  cv_bridge
  OpenCV
)
target_include_directories(Fays_VI_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
  ${FTDPATH})
target_compile_features(Fays_VI_node PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
target_link_libraries(Fays_VI_node
  ${FAYS_ATRAK_VI_MODULE_LIB}
  Threads::Threads
  ${OpenCV_LIBS}
  ${FTD3XX_LIBRARY}
)
set_target_properties(Fays_VI_node PROPERTIES
  BUILD_WITH_INSTALL_RPATH TRUE
  INSTALL_RPATH "${CMAKE_SOURCE_DIR}/thirdparty/fays_atrak/x86_64/Release"
)

install(TARGETS Fays_VI_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
