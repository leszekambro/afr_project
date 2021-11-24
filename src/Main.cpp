#include <iostream>
#include <fstream>
#include <math.h>
#include <ros/ros.h>
#include <unistd.h>
#include <ardrone_autonomy/Navdata.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <aruco_ros/aruco_ros_utils.h>
#include <aruco_msgs/MarkerArray.h>
#include <aruco_msgs/Marker.h>
#include <geometry_msgs/PoseStamped.h>
#include <visualization_msgs/Marker.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Empty.h>
#include <gazebo_msgs/ModelStates.h>
#include <dynamic_reconfigure/server.h>
#include <ardrone_control_reconf/paramsConfig.h>

using namespace std;
ardrone_autonomy::Navdata::ConstPtr navdata;


static bool isNavdataReady = false;
static bool isDroneReady = false;

float kp=1; //proportional gain for altitude controller and dynamic reconfigure
float h_zad=2; // desired value for altitude control loop and dynamic reconfigure

float pose_x_drone = 0;
float pose_y_drone = 0;

//Callbacks for getting params
void reconfigureCallback(ardrone_control_reconf::paramsConfig &config, uint32_t level){
	ROS_INFO("New values: [%f] - [%f]", config.h_zad, config.kp);
	kp = config.kp;
	h_zad= config.h_zad;
}
	

void ardroneNavdataCallback(const ardrone_autonomy::Navdata::ConstPtr& msg){
  navdata = msg;
  isNavdataReady = true;
}

void DronePoseCallback(const gazebo_msgs::ModelStates::ConstPtr& msg){
  pose_x_drone = msg->pose[11].position.x;//check drone object no in Gazebo
  pose_y_drone = msg->pose[11].position.y;
  isDroneReady = true;
}

// main loop 
int main(int argc, char* argv[])
{
  cout<<"Ros Ardrone Initialize"<<endl;
  ros::init(argc, argv, "afr_control_reconf");
  ros::NodeHandle nh;

    
  ros::Rate loop_rate(10);
  ros::spinOnce();

  ros::Subscriber subscriberNavdata = nh.subscribe("/ardrone/navdata", 1, ardroneNavdataCallback);
  ros::Subscriber subscriberDrone = nh.subscribe("/gazebo/model_states", 1, DronePoseCallback);
  ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
  dynamic_reconfigure::Server<ardrone_control_reconf::paramsConfig> server;
  dynamic_reconfigure::Server<ardrone_control_reconf::paramsConfig>::CallbackType f;

  f = boost::bind(&reconfigureCallback, _1, _2);
  server.setCallback(f);
 
  ros::spinOnce();
  
  // save to file prearing - change your user name/ account name 
  std::ofstream File ("/home/ardrone_cat/test.txt",std::ofstream::out);

  while (ros::ok()){
    if(!isNavdataReady){
            
      ros::spinOnce();
      loop_rate.sleep();

      continue;
    }

    
   // altitude controller 
   float h_m = navdata->altd/1000; // altitude reading and conversion form mm to m
   float error_h = h_zad - h_m; // altitude error calculation
   float effort_z = kp * error_h; // control effort calculation with only P controller
   /*
    * 
    * 
    * 
    * 
    * 
    * another control loops of drone local control system
    * 
    * 
    * 
    * 
    * 
    */
   
   
   
   // sending control commands  - linear and angular vel. 
   geometry_msgs::Twist msg;
   
   
   
    //msg.linear.x = ; //sending linear vel x
    //msg.linear.y = ; // sending linear vel y
    msg.linear.z = effort_z; // sending linear vel z - equal to control effort
    msg.angular.z = 0; // sending angular vel. z 
    
 
    // display in terminal some params if needed
    cout << "ster: "<< effort_z <<setprecision(2)<< endl;
    // save some data to file if needed
    File << "X: " << navdata->ax << " Y: " << navdata->ay << endl;
    
    // publish message with controls
    publisher.publish(msg);
    

    ros::spinOnce();
    loop_rate.sleep();
  }

  File.close();
  return 0;
}
