#define debug

// https://blog.csdn.net/u012700322/article/details/51821249
// C++ 标准库
#include "depthHandle.hpp"

using namespace std;

DepthHandle::DepthHandle(){

}

DepthHandle::~DepthHandle(){

}

int DepthHandle::ReadConfig(string configFileName)
{
    ifstream configFile(configFileName.c_str());
    if(!configFile.is_open()){
        cerr<<"open failed !"<< endl;
        return -1;
    }
    configFile >> camera_factor >> camera_cx >> camera_cy >> camera_fx >> camera_fy >> MinDepth >> MaxDepth >> CutRate;
    cout<< camera_factor << camera_cx << camera_cy << camera_fx << camera_fy<< MinDepth << MaxDepth << CutRate;
    configFile.close();
    return 0;
}

int DepthHandle::ReadDepthImage(string depthImageName)
{
    // depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
    depthMat = cv::imread(depthImageName, -1);
    if(!depthMat.rows * depthMat.cols){
        cerr<<"open depth file err"<<endl;
        return -1;
    }
    cout<< " depth.size " << depthMat.size() << endl;
    return 0;
    // debug  imshow("图像", depthMat);
}

double SpaceFactor(int x1, int y1, int x2, int y2, double sigmaD) {
    double absX = pow(abs(x1 - x2), 2);
    double absY = pow(abs(y1 - y2), 2);

    return exp(-(absX + absY) / (2 * pow(sigmaD, 2)));
}

double ColorFactor(int x, int y, double sigmaR) { // if it is point3d should be distance between point
    double distance = abs(x - y) / sigmaR;
    return exp(-0.5 * pow(distance, 2));
}

cv::Mat BilateralFilter3f(cv::Mat inputImg, int filterSize, double sigmaR, double sigmaD) {
    int len; //must be odd number
    cv::Mat gray; // must be 1-channel image
    cv::Mat LabImage; // if channels == 3

    if (filterSize % 2 != 1 || filterSize <= 0) {
        std::cerr << "Filter Size must be a positive odd number!" << std::endl;
        return inputImg;
    }
    len = filterSize / 2;

    if (inputImg.channels() >= 3) {
        cv::cvtColor(inputImg, LabImage, cv::COLOR_BGR2Lab);
        gray = cv::Mat::zeros(LabImage.size(), CV_8UC1);
        for (int i = 0; i < LabImage.rows; i++) {
            for (int j = 0; j < LabImage.cols; j++) {
                gray.ptr<uchar>(i)[j] = LabImage.ptr<uchar>(i, j)[0];
            }
        }
    }
    else if(inputImg.channels() == 1){
        inputImg.copyTo(gray);
    }
    else {
        std::cerr << "the count of input image's channel can not be 2!" << std::endl;
        return inputImg;
    }

    cv::Mat resultGrayImg = cv::Mat::zeros(gray.size(), CV_8UC1);
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            double k = 0;
            double f = 0;
            for (int r = i - len; r <= i + len; r++) {
                for (int c = j - len; c <= j + len; c++) {
                    if (r < 0 || c < 0 || r >= gray.rows || c >= gray.cols)
                        continue;
                    f = f + gray.ptr<uchar>(r)[c] * SpaceFactor(i, j, r, c, sigmaD) * ColorFactor(gray.ptr<uchar>(i)[j], gray.ptr<uchar>(r)[c], sigmaD);
                    k += SpaceFactor(i, j, r, c, sigmaD) * ColorFactor(gray.ptr<uchar>(i)[j], gray.ptr<uchar>(r)[c], sigmaD);
                }
            }
            int value = f / k;
            if (value < 0) value = 0;
            else if (value > 255) value = 255;

            resultGrayImg.ptr<uchar>(i)[j] = (uchar)value;
        }
    }

    cv::Mat resultImg;
    if (inputImg.channels() >= 3) {
        for (int i = 0; i < LabImage.rows; i++) {
            for (int j = 0; j < LabImage.cols; j++) {
                LabImage.ptr<uchar>(i, j)[0] = resultGrayImg.ptr<uchar>(i)[j];
            }
        }
        cv::cvtColor(LabImage, resultImg, cv::COLOR_Lab2BGR);
    }
    else {
        resultGrayImg.copyTo(resultImg);
    }

    return resultImg;
}

int DepthHandle::BilateralFilter()
{
    //bilateralFilter(depthMat, depthMat, g_ndValue, g_nsigmaColorValue, g_nsigmaSpaceValue);
   // OpenCV Error: Assertion failed ((src.type() == CV_8UC1 || src.type() == CV_8UC3) && src.type() == dst.type() && src.size() == dst.size() && src.data != dst.data) in bilateralFilter_8u, file /build/opencv-ys8xiq/opencv-2.4.9.1+dfsg/modules/imgproc/src/smooth.cpp, line 1925
    depthMat = BilateralFilter3f(depthMat, 15, 12.5, 10);
    // debug   imshow("双边滤波图像", depthMat);
    cv::imwrite("debugFilter.png", depthMat);
}

int DepthHandle::Depth2cloud(string outCloudName)
{
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    PointCloud::Ptr cloud (new PointCloud);

    // 遍历深度图
    for (int r = 0; r < depthMat.rows; r++)
    {
        for (int c = 0; c < depthMat.cols; c++)
        {
            // 获取深度图中(r,c)处的值
            double d = depthMat.ptr<unsigned char>(r)[c];
            // d 可能没有值，若如此，跳过此点

            PointT p;
 
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;

            //delete useless point
            if ( p.z < MinDepth || p.z > MaxDepth 
            || r < depthMat.rows * CutRate || r > depthMat.rows * (1-CutRate)
            || c < depthMat.cols * CutRate || c > depthMat.cols * (1-CutRate)  )
            {
                continue;
            }

            p.x = (c - camera_cx) * p.z / camera_fx;
            p.y = (r - camera_cy) * p.z / camera_fy;
 
            // 把p加入到点云中
            //if( isRightpoint(p) )
            //{
                cloud->points.push_back( p );
            // }
            
        }
    }

    #ifdef gydebugDirectionXYZ
        PointT p0(0, 0, 0);
        PointT p1(1, 0, 0);
        PointT p2(0, 2, 0);
        PointT p3(0, 0, 3);
        
        cloud->points.push_back( p0 );
        cloud->points.push_back( p1 );
        cloud->points.push_back( p2 );
        cloud->points.push_back( p3 );

        // ofstream imgFo("test.txt");
        // for (int y = 0; y < depth.rows; y++)
        // {
        //     for (int x = 0; x < depth.cols; x++)
        //     {
        //         double d = depth.ptr<unsigned char>(y)[x];
        //         imgFo <<" " << d;
        //     }
        //     imgFo << endl;
        // }
        // imgFo.close();
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

int DepthHandle::Depth2cloudNet(string outCloudName)
{
    //pointMat = new IDPoint[depthMat.cols*depthMat.rows];
    //pointMat
    // 点云变量
    // 使用智能指针，创建一个空点云。这种指针用完会自动释放。
    PointCloud::Ptr cloud (new PointCloud);

    // 遍历深度图
    for (int r = 0; r < depthMat.rows; r++)
    {
        for (int c = 0; c < depthMat.cols; c++)
        {
            // 获取深度图中(r,c)处的值
            double d = depthMat.ptr<unsigned char>(r)[c];
            // d 可能没有值，若如此，跳过此点

            PointT p;
 
            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;

            //delete useless point
            if ( p.z < MinDepth || p.z > MaxDepth 
            || r < depthMat.rows * CutRate || r > depthMat.rows * (1-CutRate)
            || c < depthMat.cols * CutRate || c > depthMat.cols * (1-CutRate)  )
            {
                continue;
            }

            p.x = (c - camera_cx) * p.z / camera_fx;
            p.y = (r - camera_cy) * p.z / camera_fy;
 
            // 把p加入到点云中
            //if( isRightpoint(p) )
            //{
                cloud->points.push_back( p );
            // }
            
        }
    }

    #ifdef gydebugDirectionXYZ
        PointT p0(0, 0, 0);
        PointT p1(1, 0, 0);
        PointT p2(0, 2, 0);
        PointT p3(0, 0, 3);
        
        cloud->points.push_back( p0 );
        cloud->points.push_back( p1 );
        cloud->points.push_back( p2 );
        cloud->points.push_back( p3 );

        // ofstream imgFo("test.txt");
        // for (int y = 0; y < depth.rows; y++)
        // {
        //     for (int x = 0; x < depth.cols; x++)
        //     {
        //         double d = depth.ptr<unsigned char>(y)[x];
        //         imgFo <<" " << d;
        //     }
        //     imgFo << endl;
        // }
        // imgFo.close();
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


bool isRightpoint(PointT p)
{
#ifdef debug1
    //cout << " p " << p.x << " " << p.y << " " << p.z <<endl;
#endif
    return true;
}


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