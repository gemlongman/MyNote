#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
using namespace std;
using namespace cv;
 
//定义全局变量
const int g_ndMaxValue = 100;
const int g_nsigmaColorMaxValue = 200;
const int g_nsigmaSpaceMaxValue = 200;
int g_ndValue=10;
int g_nsigmaColorValue = 10;
int g_nsigmaSpaceValue = 10;
 
Mat g_srcImage;
Mat g_dstImage;
 
//定义回调函数
void on_bilateralFilterTrackbar(int, void*)
{
    bilateralFilter(g_srcImage, g_dstImage, g_ndValue, g_nsigmaColorValue, g_nsigmaSpaceValue);
    imshow("双边滤波图像", g_dstImage);
}
 
int main( int argc, char** argv )
{
    string depthName;
    //string imageName;
    string configName;
    string outName;

    // "./KinectScreenshot-Color-0.png" "./KinectScreenshot-Depth-0.png" "./pointcloud.pcd"
    if(2 == argc){
        depthName = string(argv[1]);
    }else if(3 == argc){
        depthName = string(argv[1]);
        outName = string(argv[2]);
    }else if(4 == argc){
        configName = string(argv[1]);
        depthName = string(argv[2]);
        outName = string(argv[3]);
        //readconfig(configName);
    }

    g_srcImage = imread(depthName);
 
    //判断图像是否加载成功
    if(g_srcImage.empty())
    {
        cout << "图像加载失败!" << endl;
        return -1;
    }
    else
        cout << "图像加载成功!" << endl << endl;
 
    namedWindow("原图像", WINDOW_AUTOSIZE);
    imshow("原图像", g_srcImage);
 
    //定义输出图像窗口属性和轨迹条属性
    namedWindow("双边滤波图像", WINDOW_AUTOSIZE);
    g_ndValue = 10;
    g_nsigmaColorValue = 10;
    g_nsigmaSpaceValue = 10;
 
    char dName[20];
    snprintf(dName, sizeof(dName),"邻域直径 %d", g_ndMaxValue);
 
    char sigmaColorName[20];
    snprintf(sigmaColorName, sizeof(sigmaColorName),"sigmaColor %d", g_nsigmaColorMaxValue);
 
    char sigmaSpaceName[20];
    snprintf(sigmaSpaceName,sizeof(sigmaSpaceName), "sigmaSpace %d", g_nsigmaSpaceMaxValue);
 
    //创建轨迹条
    createTrackbar(dName, "双边滤波图像", &g_ndValue, g_ndMaxValue, on_bilateralFilterTrackbar);
    on_bilateralFilterTrackbar(g_ndValue, 0);
 
    createTrackbar(sigmaColorName, "双边滤波图像", &g_nsigmaColorValue,
                     g_nsigmaColorMaxValue, on_bilateralFilterTrackbar);
    on_bilateralFilterTrackbar(g_nsigmaColorValue, 0);
 
    createTrackbar(sigmaSpaceName, "双边滤波图像", &g_nsigmaSpaceValue,
                    g_nsigmaSpaceMaxValue, on_bilateralFilterTrackbar);
    on_bilateralFilterTrackbar(g_nsigmaSpaceValue, 0);
 
    waitKey(0);
 
    return 0;
}