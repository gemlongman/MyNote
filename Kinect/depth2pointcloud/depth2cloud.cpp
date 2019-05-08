#define debug

// https://blog.csdn.net/u012700322/article/details/51821249
// C++ 标准库
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;
 
// OpenCV 库
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
 
// PCL 库
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#define gydebug

// 定义点云类型
typedef pcl::PointXYZRGBA PointCT;
typedef pcl::PointCloud<PointCT> PointCloudC; 

typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloud; 

// 相机内参

// my demo
// double camera_factor = 1000;
// double camera_cx = 512.0;
// double camera_cy = 512.0;
// double camera_fx = 1024.0;
// double camera_fy = 1024.0;


/*
// my kinect
const double camera_factor = 1000;
const double camera_cx = 256.0;
const double camera_cy = 212.0;
const double camera_fx = 512.0;
const double camera_fy = 424.0;
*/

//kinect
double camera_factor = 1000;
double camera_cx = 325.5;
double camera_cy = 253.5;
double camera_fx = 518.0;
double camera_fy = 519.0;

double MinDepth = 10;
double MaxDepth = 30;
double CutRate = 0.15;

// double MinDepth = 0;
// double MaxDepth = 300;
// double CutRate = 0;

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

void readconfig(string configFileName="depth2cloudconfig.txt")
{
    ifstream configFile(configFileName.c_str());
    configFile >> camera_factor >> camera_cx >> camera_cy >> camera_fx >> camera_fy >> MinDepth >> MaxDepth >> CutRate;
    cout<< camera_factor << camera_cx << camera_cy << camera_fx << camera_fy<< MinDepth << MaxDepth << CutRate;
    configFile.close();
}

bool isRightpoint(PointT p)
{
#ifdef debug1
    //cout << " p " << p.x << " " << p.y << " " << p.z <<endl;
#endif
    return true;
}

#ifdef gydebug
void  WritetxtImage(cv::Mat & imgbuf, int width, int height)
{
	// byte
	ofstream imgFo("test.txt");
	if (!imgFo)
	{
		cerr << "open error!" << endl;
		abort();
	}
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			imgFo <<" \t " << imgbuf.ptr<uchar>(y)[x];;
		}
		imgFo << endl;
	}
	imgFo.close();
}
#endif

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
    cout<< " depth.size " << depth.size() << endl;
    #ifdef gydebugn
     WritetxtImage(depth, depth.cols, depth.rows );
    #endif
    // 遍历深度图
    for (int r = 0; r < depth.rows; r++)
    {
        for (int c = 0; c < depth.cols; c++)
        {
            // 获取深度图中(r,c)处的值
            double d = depth.ptr<unsigned char>(r)[c];
            // d 可能没有值，若如此，跳过此点

            PointT p;
 
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;

            //delete useless point
            if ( p.z < MinDepth || p.z > MaxDepth 
            || r < depth.rows * CutRate || r > depth.rows * (1-CutRate)
            || c < depth.cols * CutRate || c > depth.cols * (1-CutRate)  )
            {
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
    }
    #ifdef gydebugn
        PointT p0(0, 0, 0);
        PointT p1(1, 0, 0);
        PointT p2(0, 2, 0);
        PointT p3(0, 0, 3);
        
        cloud->points.push_back( p0 );
        cloud->points.push_back( p1 );
        cloud->points.push_back( p2 );
        cloud->points.push_back( p3 );

        ofstream imgFo("test.txt");
        for (int y = 0; y < depth.rows; y++)
        {
            for (int x = 0; x < depth.cols; x++)
            {
                double d = depth.ptr<unsigned char>(y)[x];
                imgFo <<" " << d;
            }
            imgFo << endl;
        }
        imgFo.close();
    #endif

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
    //string imageName;
    string configName;
    string pcdName;

    // "./KinectScreenshot-Color-0.png" "./KinectScreenshot-Depth-0.png" "./pointcloud.pcd"
    if(3 == argc){
        depthName = string(argv[1]);
        pcdName = string(argv[2]);
        depth2cloud(depthName,pcdName);
    }else if(4 == argc){
        configName = string(argv[1]);
        depthName = string(argv[2]);
        pcdName = string(argv[3]);
        readconfig(configName);
        // depthName = string(argv[1]);
        // imageName = string(argv[2]);
        // pcdName = string(argv[3]);
        depth2cloud(depthName,pcdName);
        //depth2cloud(depthName,imageName,pcdName);
    }
    return 0;
}
