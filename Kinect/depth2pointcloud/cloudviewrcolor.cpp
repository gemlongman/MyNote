#include <iostream>
#include <string>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>

using namespace std;

typedef pcl::PointXYZRGBA PointCT;
typedef pcl::PointCloud<PointCT> PointCloudCT;

typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloudT;

int visualCloudViewer(string cloudname,bool isColor = false)
{
    if("" == cloudname)
    {
        cloudname="pointcloud.pcd";
    }  

    if(isColor)
    {
        PointCloudCT::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
        if (pcl::io::loadPCDFile<PointCT>(cloudname, *cloud )== -1)
        {
            PCL_ERROR("Couldn't read file \n");
            return(-1);
        }
        std::cout << "Loaded " << cloud->width * cloud->height << " data points" << std::endl;

        pcl::visualization::CloudViewer viewer("viewer");
        viewer.showCloud(cloud);

        while (!viewer.wasStopped())
        {       
        }
    }
    else
    {
        PointCloudT::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
        if (pcl::io::loadPCDFile<PointT>(cloudname, *cloud )== -1)
        {
            PCL_ERROR("Couldn't read file \n");
            return(-1);
        }
        std::cout << "Loaded " << cloud->width * cloud->height << " data points" << std::endl;

        pcl::visualization::CloudViewer viewer("viewer");
        viewer.showCloud(cloud);

        while (!viewer.wasStopped())
        {       
        }
    }




    
    return 0;
}

int main(int argc,char* argv[])
{
    string cloudname;
    bool isColor(false);
    printf("argc %d",argc);// argc =2 name 3: name color=1
    if(2 == argc){
        cloudname = string(argv[1]);
    }else if(3 == argc){
        cloudname = string(argv[1]);
        isColor = ( "1" == string(argv[2]) || "true" == string(argv[2]) );
    }
    visualCloudViewer(cloudname,isColor);
    
    return 0;
}

// https://blog.csdn.net/u014801811/article/details/79711773 
/*

./cloudview
./cloudview mypointcloud.pcd
./cloudview mypointcloud.pcd 1
*/