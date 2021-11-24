#include <iostream>
#include <fstream>
#include <math.h>
#include "ros/ros.h"
#include "unistd.h"
#include "ardrone_autonomy/Navdata.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <aruco_ros/aruco_ros_utils.h>
#include <aruco_msgs/MarkerArray.h>
#include <aruco_msgs/Marker.h>
#include "geometry_msgs/PoseStamped.h"
#include "visualization_msgs/Marker.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Empty.h"
#include "gazebo_msgs/ModelStates.h"
#include <dynamic_reconfigure/server.h>
#include <ardrone_control_reconf/paramsConfig.h>

using namespace std;
ardrone_autonomy::Navdata::ConstPtr navdata;


static bool isNavdataReady = false;
static bool isPoseReady = false;
static bool isMarkerReady = false;
static bool isCartReady = false;

float kp=1;
float h_zad=2;

float pose_x = 0;
float pose_y = 0;
float pose_z = 0;
float mid = 0;
float pose_x_cart;
float pose_y_cart;

void reconfigureCallback(ardrone_control_reconf::paramsConfig &config, uint32_t level){
	ROS_INFO("New values: [%f] - [%f]", config.h_zad, config.kp);
	kp = config.kp;
	h_zad= config.h_zad;
}
	

void ardroneNavdataCallback(const ardrone_autonomy::Navdata::ConstPtr& msg){
  navdata = msg;
  isNavdataReady = true;
}



int main(int argc, char* argv[])
{
  cout<<"Ros Ardrone Initialize"<<endl;
  ros::init(argc, argv, "afr_control_reconf");
  ros::NodeHandle nh;

    
  ros::Rate loop_rate(10);
  ros::spinOnce();

  ros::Subscriber subscriberNavdata = nh.subscribe("/ardrone/navdata", 1, ardroneNavdataCallback);
  //ros::Publisher publisher = nh.advertise<std_msgs::Float32>("/fi", 1000);
  ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
  dynamic_reconfigure::Server<ardrone_control_reconf::paramsConfig> server;
  dynamic_reconfigure::Server<ardrone_control_reconf::paramsConfig>::CallbackType f;

	f = boost::bind(&reconfigureCallback, _1, _2);
	server.setCallback(f);
 
  ros::spinOnce();

  std::ofstream File ("/home/ardrone_cat/test.txt",std::ofstream::out);

  while (ros::ok()){
    if(!isNavdataReady){
            
      ros::spinOnce();
      loop_rate.sleep();

      continue;
    }

    
   
   float h_m = navdata->altd/1000;
   float error_h = h_zad - h_m;
   float effort_z = kp * error_h;
   /*
    * control system
    * 
    */
   
   float ster_x = 0; 
   float ster_y = 0;
   
   
   // sending control commands 
   geometry_msgs::Twist msg;
   
   
   
    //msg.linear.x = effort2;
    //msg.linear.y = ster_y;
   
    msg.linear.z = effort_z;
   
    //cout << "X: " << navdata->ax << " Y: " << navdata->ay << endl;
    
    cout << "ster: "<< effort_z <<setprecision(2)<< endl;
    File << "X: " << navdata->ax << " Y: " << navdata->ay << endl;
    
    
    //std_msgs::Float32 msg;
    //msg.data = fi;
    publisher.publish(msg);
    

    ros::spinOnce();
    loop_rate.sleep();
  }

  File.close();
  return 0;
}
