#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>



typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloudT;

int main(int argc,char* argv[])
{
    PointCloudT::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    if (pcl::io::loadPCDFile<PointT>("pointcloud.pcd", *cloud )== -1)
    {
        PCL_ERROR("Couldn't read file pointcloud.pcd\n");
        return(-1);
    }
    std::cout << "Loaded "
        << cloud->width*cloud->height
        << " data points from pointcloud.pcd with the following fields: "
        << std::endl;

    pcl::visualization::CloudViewer viewer("viewer");
    viewer.showCloud(cloud);
    while (!viewer.wasStopped())
    {       
    }
    system("pause");
    return 0;
}

// https://blog.csdn.net/u014801811/article/details/79711773 
