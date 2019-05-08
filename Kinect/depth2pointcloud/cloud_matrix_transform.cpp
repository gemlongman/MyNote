#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/console/parse.h>
#include <pcl/common/transforms.h>
#include <pcl/visualization/pcl_visualizer.h>

#define notshow

#define gydebug

using namespace std;

// This function displays the help
void showHelp(char * program_name)
{
  std::cout << std::endl;
  std::cout << "Usage: " << program_name << " config.txt cloud_filename.[pcd|ply] out_cloud_filename.[pcd|ply] [-Matrix] [-UnShow]" << std::endl;
  std::cout << "M right.x y z  up.x y z CameraDirection.x y z"
            <<"P x y z"
            <<"or"
            <<"'T|t' x y z"
            <"'R|r' 'X|Y|Z' angle";
  std::cout << std::endl;
  std::cout << "-h:  Show this help." << std::endl;
}

//get config txt
/*
'T|t' x y z
'R|r' 'x|y|z' angle
*/
Eigen::Affine3f GetTransAffine3f(string configFileName)
{
  Eigen::Affine3f transform = Eigen::Affine3f::Identity();

  ifstream configFile(configFileName.c_str());
  //configFile.open(configFileName);
  if(!configFile.is_open())
  {
    cerr << "cannot open " << configFile << endl;
    return transform;
  }

  char transType;
  double disX,disY,disZ,angle;
  string tempStr;
  while(configFile.get(transType))
  {
    getline(configFile ,tempStr);
    if('T'==transType || 't'==transType)
    {

      sscanf(tempStr.c_str()," %lf %lf %lf",&disX,&disY,&disZ);
      transform.translation() << disX, disY, disZ;

    }
    else if('R'==transType || 'r'==transType)
    {
      sscanf(tempStr.c_str()," %c %lf",&transType,&angle);
      angle = M_PI * angle / 180.0;
      if('X'==transType || 'x'==transType)
      {
        transform.rotate (Eigen::AngleAxisf (angle, Eigen::Vector3f::UnitX()));
      }
      else if('Y'==transType || 'y'==transType)
      {
        transform.rotate (Eigen::AngleAxisf (angle, Eigen::Vector3f::UnitY()));
      }
      else if('Z'==transType || 'z'==transType)
      {
        transform.rotate (Eigen::AngleAxisf (angle, Eigen::Vector3f::UnitZ()));
      }
      else
      {
        cerr << "error Angle Axis " << transType << endl;
      }
    }
    else if('M'==transType || 'm'==transType)
    {
      cerr << "error config file " << endl;
    }
    else
    {
      cerr << "error config file " << endl;
    }
  }
  configFile.close();

  // Print the transformation
  printf ("\nusing an Affine3f\n");
  std::cout << transform.matrix() << std::endl;

  return transform;
}


Eigen::Matrix4f  GetTransMatrix4f(string configFileName)
{
  Eigen::Matrix4f  transform = Eigen::Matrix4f::Identity();

  ifstream configFile(configFileName.c_str());
  //configFile.open(configFileName);
  if(!configFile.is_open())
  {
    cerr << "cannot open " << configFile << endl;
    return transform;
  }

  char transType;
  double temp[3][3];
  string tempStr;

  configFile >> transType;
  if('M'==transType || 'm'==transType)
  {
    // configFile 
    // >> transform(0,0) >> transform(0,1) >> transform(0,2)
    // >> transform(1,0) >> transform(1,1) >> transform(1,2) 
    // >> transform(2,0) >> transform(2,1) >> transform(2,2);

    configFile 
    >> transform(0,0) >> transform(1,0) >> transform(2,0)
    >> transform(0,1) >> transform(1,1) >> transform(2,1) 
    >> transform(0,2) >> transform(1,2) >> transform(2,2);
  }
  else
  {
    cerr << "error config file " << endl;
  }

  configFile >> transType; 

  if('P'==transType || 'p'==transType || 'T'==transType || 't'==transType)
  {
    
    configFile >> transform(0,3) >> transform(1,3) >> transform(2,3); 
    // transform(0,3)=-transform(0,3);
    // transform(1,3)=-transform(1,3);
    // transform(2,3)=-transform(2,3);
  }

  printf ("Method #1: using a Matrix4f\n");
  std::cout << transform << std::endl;

  configFile.close();
  return transform;
}

// This is the main function
int main (int argc, char** argv)
{

  // Show help
  if (pcl::console::find_switch (argc, argv, "-h") || pcl::console::find_switch (argc, argv, "--help")) {
    showHelp (argv[0]);
    return 0;
  }
  int isNeedShow=1;
  int isUseMatrix=0;

  if(pcl::console::find_argument(argc, argv, "-Matrix")>0)
  {
    isUseMatrix=1;
  }

  if(pcl::console::find_argument(argc, argv, "-UnShow")>0)
  {
    isNeedShow=0;
  }

  // Fetch point cloud filename in arguments | Works with PCD and PLY files
  std::vector<int> filenames;
  std::vector<int> configfilenames;
  bool file_is_pcd = false;

  filenames = pcl::console::parse_file_extension_argument (argc, argv, ".ply");
  configfilenames = pcl::console::parse_file_extension_argument (argc, argv, ".txt");

  if (filenames.size () < 1)  {
    filenames = pcl::console::parse_file_extension_argument (argc, argv, ".pcd");

    if (filenames.size () < 1) {
      showHelp (argv[0]);
      return -1;
    } else {
      file_is_pcd = true;
    }
  }

  // Load file | Works with PCD and PLY files
  pcl::PointCloud<pcl::PointXYZ>::Ptr source_cloud (new pcl::PointCloud<pcl::PointXYZ> ());

  if (file_is_pcd) {
    if (pcl::io::loadPCDFile (argv[filenames[0]], *source_cloud) < 0)  {
      std::cout << "Error loading point cloud " << argv[filenames[0]] << std::endl << std::endl;
      showHelp (argv[0]);
      return -1;
    }
  } else {
    if (pcl::io::loadPLYFile (argv[filenames[0]], *source_cloud) < 0)  {
      std::cout << "Error loading point cloud " << argv[filenames[0]] << std::endl << std::endl;
      showHelp (argv[0]);
      return -1;
    }
  }

  /* Reminder: how transformation matrices work :

           |-------> This column is the translation
    | 1 0 0 x |  \
    | 0 1 0 y |   }-> The identity 3x3 matrix (no rotation) on the left
    | 0 0 1 z |  /
    | 0 0 0 1 |    -> We do not use this line (and it has to stay 0,0,0,1)

    METHOD #1: Using a Matrix4f
    This is the "manual" method, perfect to understand but error prone !
  */

  pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud (new pcl::PointCloud<pcl::PointXYZ> ());

  if(isUseMatrix)
  {
    Eigen::Matrix4f transform = GetTransMatrix4f(argv[configfilenames[0]]);
    pcl::transformPointCloud (*source_cloud, *transformed_cloud, transform);
  }
  else
  {
    Eigen::Affine3f transform = GetTransAffine3f(argv[configfilenames[0]]);
    pcl::transformPointCloud (*source_cloud, *transformed_cloud, transform);
  }
  
   



  // 设置并保存点云
  // transformed_cloud->height = 1;
  // transformed_cloud->width = transformed_cloud->points.size();
  cout<<"point cloud size = "<<transformed_cloud->points.size()<<endl;
  // transformed_cloud->is_dense = false;
  pcl::io::savePCDFile( argv[filenames[1]], *transformed_cloud );
  cout<<"Point cloud saved."<<endl;

  if(isNeedShow)
  {
    // Visualization
    printf(  "\nPoint cloud colors :  white  = original point cloud\n"
        "                        red  = transformed point cloud\n");
    pcl::visualization::PCLVisualizer viewer ("Matrix transformation example");

    // Define R,G,B colors for the point cloud
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> source_cloud_color_handler (source_cloud, 255, 255, 255);
    // We add the point cloud to the viewer and pass the color handler
    viewer.addPointCloud (source_cloud, source_cloud_color_handler, "original_cloud");

    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> transformed_cloud_color_handler (transformed_cloud, 230, 20, 20); // Red
    viewer.addPointCloud (transformed_cloud, transformed_cloud_color_handler, "transformed_cloud");

    viewer.addCoordinateSystem (1.0, "cloud", 0);
    viewer.setBackgroundColor(0.05, 0.05, 0.05, 0); // Setting background to a dark grey
    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "original_cloud");
    viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "transformed_cloud");
    //viewer.setPosition(800, 400); // Setting visualiser window position

    while (!viewer.wasStopped ()) { // Display the visualiser until 'q' key is pressed
      viewer.spinOnce ();
    }
  }


  return 0;
}

