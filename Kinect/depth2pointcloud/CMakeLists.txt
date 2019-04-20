cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(depth2cloud)

find_package(PCL 1.2 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${PCL_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS} ${OPENCV_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS} ${OPENCV_DEFINITIONS})

list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")

add_executable (csdndepth2cloud csdndepth2cloud.cpp)
#add_executable (cloudview cloudviewrcolor.cpp)
#add_executable (cloud2mesh cloud2mesh.cpp)
add_executable (cloudresampling resampling.cpp)
#add_executable (cloud2planemesh cloud2planemesh.cpp)
add_executable (cloud2meshview cloud2meshview.cpp)

target_link_libraries (csdndepth2cloud ${PCL_LIBRARIES} ${OpenCV_LIBS})
#target_link_libraries (cloudview ${PCL_LIBRARIES} ${OpenCV_LIBS})
#target_link_libraries (cloud2mesh ${PCL_LIBRARIES})
target_link_libraries (cloudresampling ${PCL_LIBRARIES})
#target_link_libraries (cloud2planemesh ${PCL_LIBRARIES})
target_link_libraries (cloud2meshview ${PCL_LIBRARIES})