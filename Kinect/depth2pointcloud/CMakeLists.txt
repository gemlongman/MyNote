cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(depth2mesh)

#set(CMAKE_CXX_STANDARD 11)

find_package(PCL 1.2 REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${PCL_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS} ${OPENCV_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS} ${OPENCV_DEFINITIONS})

list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")

add_executable (depth2cloud depth2cloud.cpp)
add_executable (depthFilter depthFilter.cpp)
#add_executable (cloudview cloudviewrcolor.cpp)
add_executable (cloudresampling resampling.cpp)
add_executable (cloudtransform cloud_matrix_transform.cpp)
add_executable (cloudconcatenate concatenate_clouds.cpp)
add_executable (cloudAlign cloudPairAlign.cpp)
add_executable (cloud2meshCube recon_marchingCubes.cpp)
add_executable (cloud2meshPoisson recon_poisson.cpp)
add_executable (cloud2meshGreedy recon_greedyProjection.cpp)
add_executable (depth2meshGY depth2meshGY.cpp)

target_link_libraries (depth2cloud ${PCL_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries (depthFilter ${PCL_LIBRARIES} ${OpenCV_LIBS})
#target_link_libraries (cloudview ${PCL_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries (cloudresampling ${PCL_LIBRARIES})
target_link_libraries (cloudtransform ${PCL_LIBRARIES})
target_link_libraries (cloudconcatenate ${PCL_LIBRARIES})
target_link_libraries (cloudAlign ${PCL_LIBRARIES})
target_link_libraries (cloud2meshCube ${PCL_LIBRARIES})
target_link_libraries (cloud2meshPoisson ${PCL_LIBRARIES})
target_link_libraries (cloud2meshGreedy ${PCL_LIBRARIES})
target_link_libraries (depth2meshGY ${PCL_LIBRARIES} ${OpenCV_LIBS})
