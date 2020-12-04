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

void markerPoseCallback(const geometry_msgs::PoseStamped::ConstPtr& msg){
  pose_x = msg->pose.position.x;
  pose_y = msg->pose.position.y;
  pose_z = msg->pose.position.z;
  isPoseReady = true;
}

void markerCallback(const visualization_msgs::Marker::ConstPtr& msg){
  mid = msg->id;
  isMarkerReady = true;
}
void cartPoseCallback(const gazebo_msgs::ModelStates::ConstPtr& msg){
  pose_x_cart = msg->pose[11].position.x;
  pose_y_cart = msg->pose[11].position.y;
  isCartReady = true;
}

int main(int argc, char* argv[])
{
  cout<<"Ros Ardrone Initialize"<<endl;
  ros::init(argc, argv, "ardrone_control_reconf");
  ros::NodeHandle nh;

    
  ros::Rate loop_rate(10);
  ros::spinOnce();

  ros::Subscriber subscriberNavdata = nh.subscribe("/ardrone/navdata", 1, ardroneNavdataCallback);
  ros::Subscriber subscriberPose = nh.subscribe("/aruco_single/pose", 1, markerPoseCallback);
  ros::Subscriber subscriberMarker = nh.subscribe("/aruco_single/marker", 1, markerCallback);
  ros::Subscriber subscriberCart = nh.subscribe("/gazebo/model_states", 1, cartPoseCallback);
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

    geometry_msgs::Twist msg;
   
   float h_m = navdata->altd/1000;
   /*
    * Kod sterowania
    * 
    */
   
   float ster_x = 0; 
   float ster_y = 0;
   
   //msg.linear.z = ster;
    msg.linear.x = ster_x;
    msg.linear.y = ster_y;
    //cout << "X: " << navdata->ax << " Y: " << navdata->ay << endl;
    
    cout << "ster: "<< ster_x <<setprecision(2)<< endl;
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
