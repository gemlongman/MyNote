// https://blog.csdn.net/u012700322/article/details/51821249
// C++ 标准库
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

//using namespace std;
 
// OpenCV 库
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
// PCL 库
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#define gydebug
//#define gydebugDirectionXYZ

// 定义点云类型
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloud; 

typedef struct{
    pcl::PointXYZ p;
    int id;
}IDPoint;

class DepthHandle
{
public:    
    DepthHandle();
    ~DepthHandle();
    int ReadConfig(std::string configFileName = "depth2cloudconfig.txt");
    int ReadDepthImage( std::string depthImageName);
    int BilateralFilter();
    int Depth2cloud(std::string outCloudName);
    int Depth2cloudNet(std::string outCloudName);

    
private:
    cv::Mat depthMat;
    //IDPoint ** pointMat;
    std::vector< std::vector<IDPoint> > pointMat;
    // 相机内参

    // my demo
    double camera_factor = 1000;
    double camera_cx = 512.0;
    double camera_cy = 512.0;
    double camera_fx = 1024.0;
    double camera_fy = 1024.0;
    double MinDepth = 0;
    double MaxDepth = 300;
    double CutRate = 0;

    int g_ndValue=10;
    int g_nsigmaColorValue = 10;
    int g_nsigmaSpaceValue = 10;

    /*
    // my kinect
    const double camera_factor = 1000;
    const double camera_cx = 256.0;
    const double camera_cy = 212.0;
    const double camera_fx = 512.0;
    const double camera_fy = 424.0;
    */

    //kinect
    // double camera_factor = 1000;
    // double camera_cx = 325.5;
    // double camera_cy = 253.5;
    // double camera_fx = 518.0;
    // double camera_fy = 519.0;

    // double MinDepth = 10;
    // double MaxDepth = 30;
    // double CutRate = 0.15;
};
