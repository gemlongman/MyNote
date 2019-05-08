#/bin/bash

mkdir build 
cd build
cmake ..
make

#rm -f *.pcd 
#rm -f *.vtk *.obj

# ./depth2cloud KinectShot-Color-0.png  KinectShot-Depth-0.png pointcloud1.pcd

./depth2cloud depth2cloudconfig.txt KinectShot-Depth-26.png pointcloud.pcd

pcl_viewer pointcloud.pcd

#./cloudresampling
#cp pointcloud.pcd pointcloud_re.pcd

pcl_viewer pointcloud_re.pcd

./cloud2meshCube pointcloud.pcd
./cloud2meshCube pointcloud_re.pcd

./cloud2meshPoisson pointcloud.pcd meshPoisson

./cloud2meshGreedy pointcloud.pcd meshGreedy
./cloud2meshGreedy pointcloud_re.pcd meshGreedy_re