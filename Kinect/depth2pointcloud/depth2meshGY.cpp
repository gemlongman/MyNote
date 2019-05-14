#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "depthHandle/depthHandle.hpp"
using namespace std;

#define gydebug
//#define gydebugDirectionXYZ


// 主函数 
int main( int argc, char** argv )
{
    string depthName;
    string configName;
    string pcdName;

    DepthHandle handle;
    // "./configName.txt" "./KinectScreenshot-Depth-0.png" "./pointcloud.pcd"
    if(3 == argc){
        depthName = string(argv[1]);
        pcdName = string(argv[2]);
    }else if(4 == argc){
        configName = string(argv[1]);
        depthName = string(argv[2]);
        pcdName = string(argv[3]);
        handle.ReadConfig(configName);
    }

    handle.ReadDepthImage(depthName);
    handle.BilateralFilter();
    handle.Depth2cloud(pcdName);

    return 0;
}
