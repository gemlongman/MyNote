#define debug

// https://blog.csdn.net/u012700322/article/details/51821249
// C++ 标准库
#include <iostream>
#include <string>

using namespace std;
 
// OpenCV 库
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
 
// PCL 库
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

// 定义点云类型
typedef pcl::PointXYZRGBA PointCT;
typedef pcl::PointCloud<PointCT> PointCloudC; 

typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloud; 

// 相机内参
/*
const double camera_factor = 1000;
const double camera_cx = 256.0;
const double camera_cy = 212.0;
const double camera_fx = 512.0;
const double camera_fy = 424.0;
*/
const double camera_factor = 1000;
const double camera_cx = 325.5;
const double camera_cy = 253.5;
const double camera_fx = 518.0;
const double camera_fy = 519.0;

const double MinDepth = 10;
const double MaxDepth = 30;
const double CutRate = 0.15;

int depth2cloud(string rgbImageName, string depthImageName, string outCloudName)
{
    // 读取./data/rgb.png和./data/depth.png，并转化为点云
 
    // 图像矩阵
    cv::Mat rgb, depth;
    // 使用cv::imread()来读取图像
    // API: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#cv2.imread
    rgb = cv::imread(rgbImageName);
    // rgb 图像是8UC3的彩色图像
    // depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
    depth = cv::imread(depthImageName, -1 );
 
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    PointCloudC::Ptr cloud ( new PointCloudC );
    // 遍历深度图
    for (int m = 0; m < depth.rows; m++)
        for (int n=0; n < depth.cols; n++)
        {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointCT p;
 
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;
            
            // 从rgb图像中获取它的颜色
            // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
            p.b = rgb.ptr<uchar>(m)[n*3];
            p.g = rgb.ptr<uchar>(m)[n*3+1];
            p.r = rgb.ptr<uchar>(m)[n*3+2];
 
            // 把p加入到点云中
            cloud->points.push_back( p );
        }
    // 设置并保存点云
    cloud->height = 1;
    cloud->width = cloud->points.size();
    cout<<"point cloud size = "<<cloud->points.size()<<endl;
    cloud->is_dense = false;
    pcl::io::savePCDFile( outCloudName, *cloud );
    // 清除数据并退出
    cloud->points.clear();
    cout<<"Point cloud saved."<<endl;
    return 0;
}

bool isRightpoint(PointT p)
{
#ifdef debug1
    cout << " p " << p.x << " " << p.y << " " << p.z <<endl;
#endif
    return true;
}

int depth2cloud(string depthImageName, string outCloudName)
{
 
    // 图像矩阵
    cv::Mat depth;
    // depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
    depth = cv::imread(depthImageName, -1);
 
    if(!depth.rows*depth.cols){
        cerr<<"open file err"<<endl;
    }
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    PointCloud::Ptr cloud (new PointCloud);

    //cout << " depth.rows " << depth.rows << " depth.cols "<<depth.cols<<endl;
    // 遍历深度图
    for (int r = 0; r < depth.rows; r++)
        for (int c = 0; c < depth.cols; c++)
        {
            // 获取深度图中(r,c)处的值
            ushort d = depth.ptr<ushort>(r)[c];
            // d 可能没有值，若如此，跳过此点


            PointT p;
 
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;

            //delete useless point
            if ( p.z < MinDepth || p.z > MaxDepth 
            || r < depth.rows * CutRate || r > depth.rows * (1-CutRate)
            || c < depth.cols * CutRate || c > depth.cols * (1-CutRate)  )
            {
            #ifdef debug1
                cout << " r c z " << r << " " << c << " " << p.z <<endl;
            #endif
                continue;
            }

            p.x = (c - camera_cx) * p.z / camera_fx;
            p.y = (r - camera_cy) * p.z / camera_fy;
 
            // 把p加入到点云中
            if( isRightpoint(p) )
            {
                cloud->points.push_back( p );
            }
            
        }
    // 设置并保存点云
    cloud->height = 1;
    cloud->width = cloud->points.size();
    cout<<"point cloud size = "<<cloud->points.size()<<endl;
    cloud->is_dense = false;
    pcl::io::savePCDFile( outCloudName, *cloud );
    // 清除数据并退出
    cloud->points.clear();
    cout<<"Point cloud saved."<<endl;
    return 0;
}

// 主函数 
int main( int argc, char** argv )
{
    string depthName;
    string imageName;
    string pcdName;

    // "./KinectScreenshot-Color-0.png" "./KinectScreenshot-Depth-0.png" "./pointcloud.pcd"
    if(3 == argc){
        depthName = string(argv[1]);
        pcdName = string(argv[2]);
        depth2cloud(depthName,pcdName);
    }else if(4 == argc){
        depthName = string(argv[1]);
        imageName = string(argv[2]);
        pcdName = string(argv[3]);
        depth2cloud(depthName,imageName,pcdName);
    }
    return 0;
}
