#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <string>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cerr << "please specify command line arg inCloudFile1 inCloudFile2 ... outCloudFile" << std::endl;
        exit(0);
    }
    pcl::PointCloud<pcl::PointXYZ> cloud_a, cloud_b, cloud_c;
    if (pcl::io::loadPCDFile(std::string(argv[1]), cloud_a) == -1){
        std::cerr << ("Could not read pcd file !" + std::string(argv[1]) + "\n" ) ;
        return -1;
    }
    cout<<"point cloud a size = "<<cloud_a.points.size()<<endl;
    cloud_a.points.resize (cloud_a.width * cloud_a.height);
    cloud_c  = cloud_a;

    int inputFileNum = argc-2;
    std::vector<std::string> inClouds;
    for(int i=2;i<=inputFileNum;i++)
    {

        if (pcl::io::loadPCDFile(std::string(argv[i]), cloud_b) == -1){
            std::cerr << ("Could not read pcd file  !"+std::string(argv[i]) + "\n") ;
            return -1;
        }
        cout<<"size = "<<cloud_b.points.size()<<endl;
        cloud_b.points.resize (cloud_b.width * cloud_b.height);
        
        cloud_c += cloud_b;
    }

    std::string outCloudFile=std::string(argv[inputFileNum+1]);

    // *cloudC  = *cloudA;
    // *cloudC += *cloudB;

    // 设置并保存点云
    //cloudC->height = 1;
    //cloudC->width = cloudC->points.size();
    cout<<"point cloud c size = "<<cloud_c.points.size()<<endl;
    //cloudC->is_dense = false;
    pcl::io::savePCDFile( outCloudFile, cloud_c );
    // 清除数据并退出
    //cloudC->points.clear();
    cout<<"Point cloud saved."<<endl;
    
  return 0;
}